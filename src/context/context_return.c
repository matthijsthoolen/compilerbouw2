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
};

#define INFO_RETURNFOUND(n) ((n)->returnFound)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_RETURNFOUND(result) = FALSE;

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
                returnStmt = stmt;
            }

            stmt = STMTS_NEXT(stmt);
        }

        TRAVopt(INNERBLOCK_STMTS(FUN_BODY(arg_node)), arg_info);

        if (INFO_RETURNFOUND(arg_info) == TRUE && stmt != NULL) {
            CTIwarn(
                "For function '%s' A return statement has been found. But it might not be reachable.",
                FUN_ID(arg_node)
            );
        } else if (returnStmt == NULL) {
            CTIerror(
                "No return statement found in a non-void function '%s'. Expected at least one return statement.",
                FUN_ID(arg_node)
            );
        } else if (stmt != NULL) {
            CTIerror(
                "It seems like there is unreachable code after the return statement in function '%s'.",
                FUN_ID(arg_node)
            );
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
