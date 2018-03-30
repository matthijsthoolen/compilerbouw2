/**
 * file: global_init.c
 *
 * If global functions or vardefs, create a __init function and add all global
 * vars and functions to the __init function.
 */

#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"

#include "global_init.h"

struct INFO {
    node* symbol_table;
    node* inner_block;
};

#define INFO_CURSYMBOLTABLE(n) ((n)->symbol_table)
#define INFO_INNERBLOCK(n) ((n)->inner_block)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_CURSYMBOLTABLE(result) = TBmakeSymboltable(NULL);
    INFO_INNERBLOCK(result) = TBmakeInnerblock(NULL, NULL);

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    free(INFO_CURSYMBOLTABLE(info));
    free(INFO_INNERBLOCK(info));

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *DSGIprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSGIprogram");

    node *innerblock = TBmakeInnerblock(NULL, NULL);

    node *declarations = arg_node;

    // Find and register all global variables first
    while (declarations != NULL) {
        if (NODE_TYPE(PROGRAM_HEAD(declarations)) == N_vardef) {
            node *varDef = PROGRAM_HEAD(declarations);

            if (VARDEF_PREFIX(varDef) == global_prefix_var && VARDEF_INIT(varDef) != NULL) {

                DBUG_PRINT("DSGIVARDEF", ("Found global vardef '%s'", VARDEF_ID(varDef)));

                node *var = TBmakeVar(STRcpy(VARDEF_ID(varDef)), NULL);

                VAR_DECL(var) = varDef;

                // Now lets do the split
                node *stmts = TBmakeStmts(
                    TBmakeAssign(var, VARDEF_INIT(varDef)),
                    NULL
                );

                // Vardef init can be removed now
                VARDEF_INIT(varDef) = NULL;

                if (INNERBLOCK_STMTS(innerblock)) {
                    node *tail = INNERBLOCK_STMTS(innerblock);
                    DBUG_PRINT("DSGIVARDEF", ("Splitting else %d", NODE_TYPE(tail)));
                    // Get to the end
                    while(STMTS_NEXT(tail)) {
                        DBUG_PRINT("DSGIVARDEF", ("Splitting vardef '%s'", VAR_NAME(ASSIGN_LEFT(STMTS_STMT(tail)))));
                        tail = STMTS_NEXT(tail);
                    }

                    STMTS_NEXT(tail) = stmts;
                } else {
                    DBUG_PRINT("DSGIVARDEF", ("Splitting emptyness"));
                    INNERBLOCK_STMTS(innerblock) = stmts;
                }
            }
            PROGRAM_HEAD(declarations) = TRAVdo(varDef, arg_info);
        }

        declarations = PROGRAM_NEXT(declarations);
    }

    construct_init(arg_node, innerblock);

    DBUG_RETURN(arg_node);
}

/** Helper function **/

void construct_init(node *syntaxtree, node *innerblock)
{
    node *current;
    node *initFun;

    DBUG_ENTER("construct_init");

    current = syntaxtree;

    // Get to the end of the declarations
    while (PROGRAM_NEXT(current) != NULL) {
        current = PROGRAM_NEXT(current);
    }

    // Generate the new __init function
    initFun = generate_init(innerblock);

    PROGRAM_NEXT(current) = TBmakeProgram(initFun, NULL, TBmakeSymboltable(TBmakeSymboltableentry(NULL)));

    DBUG_VOID_RETURN(syntaxtree);
}

node *generate_init(node *innerblock)
{
    node *initFun;

    DBUG_ENTER("generate_init");

    initFun = TBmakeFun(
                global_prefix_none,
                TY_void,
                STRcpy("__init"),
                NULL,
                innerblock,
                TBmakeSymboltable(TBmakeSymboltableentry(NULL))
            );

    DBUG_RETURN(initFun);
}

/** Start function **/

node *DSGIdoGlobalInit(node *syntaxtree)
{
    DBUG_ENTER("DSGIdoGlobalInit");

    info *info;
    info = MakeInfo();

    TRAVpush(TR_dsgi);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
