/**
 * file: context_return.c
 *
 * Check if a non-void function has a return statement.
 */

#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "memory.h"

#include "context_return.h"

struct INFO {
    bool returnFound;
    bool insideOptionalBlock;
    char * funName;
};

#define INFO_RETURNFOUND(n) ((n)->returnFound)
#define INFO_INSIDEOPTIONALBLOCK(n) ((n)->insideOptionalBlock)
#define INFO_FUNNAME(n) ((n)->funName)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_RETURNFOUND(result) = FALSE;
    INFO_INSIDEOPTIONALBLOCK(result) = FALSE;
    INFO_FUNNAME(result) = "";

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *CARCfun(node *arg_node, info *arg_info)
{
    node *returnStmt;
    node *stmt;

    DBUG_ENTER("CARCfun");

    // Dont check externs or void type functions
    if (FUN_PREFIX(arg_node) != global_prefix_extern && FUN_RETTY(arg_node) != TY_void) {

        returnStmt = NULL;
        stmt = INNERBLOCK_STMTS(FUN_BODY(arg_node));

        // Keep going until we find the return statement or no stmts left
        while (returnStmt == NULL && stmt != NULL) {
            if (NODE_TYPE(STMTS_STMT(stmt)) == N_return) {
                returnStmt = STMTS_STMT(stmt);
            }

            stmt = STMTS_NEXT(stmt);
        }

        if (returnStmt == NULL) {
            CTIerror(
                "function '%s' must end with a return of type %s",
                FUN_ID(arg_node),
                get_type_name(FUN_RETTY(arg_node))
            );
        } else {
            // Now that we are sure that this function has a return statement, let's search for other possible errors.
            // related to the return statement. E.g. Unreachable code, return not at the very end of the function.
            INFO_FUNNAME(arg_info) = FUN_ID(arg_node);
            TRAVopt(INNERBLOCK_STMTS(FUN_BODY(arg_node)), arg_info);
        }
    }

    DBUG_RETURN(arg_node);
}

node *CARCreturn(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CARCreturn");

    INFO_RETURNFOUND(arg_info) = TRUE;

    DBUG_RETURN(arg_node);
}

node *CARCif(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CARCif");

    INFO_INSIDEOPTIONALBLOCK(arg_info) = TRUE;

    TRAVopt(IF_BLOCKT(arg_node), arg_info);

    TRAVopt(IF_BLOCKF(arg_node), arg_info);

    INFO_INSIDEOPTIONALBLOCK(arg_info) = FALSE;

    DBUG_RETURN(arg_node);
}

node *CARCstmts(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CARCstmts");

    STMTS_STMT( arg_node) = TRAVdo( STMTS_STMT( arg_node), arg_info);

    if (NODE_TYPE(STMTS_STMT(arg_node)) == N_return) {
        if (INFO_INSIDEOPTIONALBLOCK(arg_info) == TRUE) {
            // Check if there is more code after a return statement inside an optional block
            if (STMTS_NEXT(arg_node) != NULL) {
                CTIwarnLine(
                    NODE_LINE(STMTS_STMT(arg_node)),
                    "Unreachable code found after return statement in function '%s'",
                    INFO_FUNNAME(arg_info)
                );
            }
        } else if (INFO_INSIDEOPTIONALBLOCK(arg_info) == FALSE) {
            // Check if there is more code after a return, if so. Give an error.
            if (STMTS_NEXT(arg_node) != NULL) {
                CTIerrorLine(
                    NODE_LINE(STMTS_STMT(arg_node)),
                    "function '%s' must end with a return statement",
                    INFO_FUNNAME(arg_info)
                );
            }
        }
    }

    STMTS_NEXT(arg_node) = TRAVopt(STMTS_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CARCdoContextReturnCheck(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("CARCdoContextReturnCheck");

    DBUG_ASSERT((syntaxtree != NULL), "CARCdoContextReturnCheck called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_carc);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
