#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "hash_list.h"
#include "list.h"
#include "copy_node.h"
#include "copy.h"
#include "free_node.h"

#include "for_to_while.h"

struct INFO {
    node *curFun;
    node *curStmt;
};

#define INFO_CURFUN(n)             ((n)->curFun)
#define INFO_CURSTMT(n)            ((n)->curStmt)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_CURFUN(result) = NULL;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *DSFWfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSFWfun");

    node *tmp = INFO_CURFUN(arg_info);
    INFO_CURFUN(arg_info) = arg_node;

    TRAVopt(FUN_BODY(arg_node), arg_info);

    INFO_CURFUN(arg_info) = tmp;

    DBUG_RETURN(arg_node);
}

node *DSFWstmts(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSFWstmts");

    INFO_CURSTMT(arg_info) = arg_node;
    TRAVdo(STMTS_STMT(arg_node), arg_info);
    TRAVopt(STMTS_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *DSFWfor(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSEfor");

    DBUG_PRINT("DSEfor", ("Point 1"));

    node *assign           = COPYdoCopy(FOR_ASSIGN(arg_node));
    node *whileLoopVar     = TBmakeVar(VAR_NAME(ASSIGN_LEFT(assign)), NULL);
    VAR_DECL(whileLoopVar) = VAR_DECL(ASSIGN_LEFT(assign));
    node *innerblock       = FUN_BODY(INFO_CURFUN(arg_info));

    DBUG_PRINT("DSEfor", ("Point 2"));

    node *step       = TBmakeVar(STRcat(VAR_NAME(whileLoopVar), "_step"), NULL);
    NODE_LINE(step)  = NODE_LINE(arg_node);
    NODE_COL(step)   = NODE_COL(arg_node);
    node *stepVardef = TBmakeVardef(0, TY_int, VAR_NAME(step), NULL, NULL, TBmakeInt(0));
    VAR_DECL(step)   = stepVardef;
    INNERBLOCK_VARS(innerblock)        = TBmakeVardeflist(stepVardef, INNERBLOCK_VARS(innerblock));
    STMTS_NEXT(INFO_CURSTMT(arg_info)) = TBmakeStmts(
        TBmakeAssign(COPYdoCopy(step), COPYdoCopy(FOR_STEP(arg_node))),
        STMTS_NEXT(INFO_CURSTMT(arg_info))
    );

    DBUG_PRINT("DSEfor", ("Point 3"));

    node *upper       = TBmakeVar(STRcat(VAR_NAME(whileLoopVar), "_done"), NULL);
    NODE_LINE(upper)  = NODE_LINE(FOR_UPPER(arg_node));
    NODE_COL(upper)   = NODE_COL(FOR_UPPER(arg_node));
    node *upperVardef = TBmakeVardef(0, TY_int, VAR_NAME(upper), NULL, NULL, TBmakeInt(0));
    VAR_DECL(upper)   = upperVardef;
    INNERBLOCK_VARS(innerblock) = TBmakeVardeflist(upperVardef, INNERBLOCK_VARS(innerblock));
    STMTS_NEXT(INFO_CURSTMT(arg_info)) = TBmakeStmts(
        TBmakeAssign(COPYdoCopy(upper), COPYdoCopy(FOR_UPPER(arg_node))),
        STMTS_NEXT(INFO_CURSTMT(arg_info))
    );

    DBUG_PRINT("DSEfor", ("Point 4"));

    node *cond  = TBmakeTernop(
        TBmakeBinop(BO_gt, COPYdoCopy(step), TBmakeInt(0)),
        TBmakeBinop(BO_lt, COPYdoCopy(whileLoopVar), COPYdoCopy(upper)),
        TBmakeBinop(BO_gt, COPYdoCopy(whileLoopVar), COPYdoCopy(upper))
    );
    TERNOP_TYPE(cond) = TY_bool;

    DBUG_PRINT("DSEfor", ("Point 5"));

    node *block = TBmakeStmts(
        TBmakeAssign(
            COPYdoCopy(whileLoopVar),
            TBmakeBinop(BO_add, COPYdoCopy(whileLoopVar), COPYdoCopy(step))
        ),
        FOR_BLOCK(arg_node)
    );

    DBUG_PRINT("DSEfor", ("Point 6"));

    // Cond (expr) block(Stmts)
    node *whileLoop = TBmakeWhile(cond, block);

    // Now the while loop is created, the old FOR loop may be removed and replaced by the new while lookup
    FOR_BLOCK(arg_node) = NULL;
    arg_node = FREEfor(arg_node, arg_info);
    STMTS_STMT(INFO_CURSTMT(arg_info)) = whileLoop;
    arg_node = whileLoop;

    DBUG_PRINT("DSEfor", ("Point 7"));

    WHILE_BLOCK(arg_node) = TRAVopt(WHILE_BLOCK(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *DSFWforToWhile(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSFWforToWhile");

    DBUG_ASSERT((syntaxtree != NULL), "DSFWforToWhile called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_dsfw);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    FreeInfo(info);

    DBUG_RETURN(syntaxtree);
}
