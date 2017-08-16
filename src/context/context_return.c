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

#include "context_return.h"

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

        if (returnStmt == NULL) {
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

node *CARCdoContextReturnCheck(node *syntaxtree)
{
    DBUG_ENTER("CARCdoContextReturnCheck");

    TRAVpush(TR_carc);

    syntaxtree = TRAVdo( syntaxtree, NULL);

    TRAVpop();

    DBUG_RETURN( syntaxtree);
}
