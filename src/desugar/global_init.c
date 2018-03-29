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
#include "list.h"

#include "global_init.h"

struct INFO {
    list *assigns;
};

#define INFO_ASSIGNS(n) ((n)->assigns)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_ASSIGNS(result) = list_new();

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *DSGIprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSGIprogram");

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);

    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *DSGIvardef(node *arg_node, info *arg_info)
{
    node *assign;

    DBUG_ENTER("DSGIvardef");

    DBUG_PRINT("HELP", ("%s", VARDEF_ID(arg_node)));

    if (VARDEF_PREFIX(arg_node) == global_prefix_var) {

        if (VARDEF_INIT(arg_node) != NULL) {
            assign = TBmakeAssign(
                        TBmakeVar(STRcpy(VARDEF_ID(arg_node)), NULL),
                        VARDEF_INIT(arg_node)
                     );

            DBUG_PRINT("HELP", ("%s", get_type_name(NODE_TYPE(assign))));

            list_push(INFO_ASSIGNS(arg_info), assign);

            // Vardef init can be removed now
            VARDEF_INIT(arg_node) = NULL;
        }

    }

    DBUG_RETURN(arg_node);
}

/** Helper function **/

void construct_init(node *syntaxtree, info *info)
{
    node *current;
    node *initFun;

    current = syntaxtree;

    // Get to the end of the declarations
    while (PROGRAM_NEXT(current) != NULL) {
        current = PROGRAM_NEXT(current);
    }

    // Generate the new __init function
    initFun = generate_init(info);

    PROGRAM_NEXT(current) = TBmakeProgram(initFun, NULL, TBmakeSymboltable(NULL));
}

node *generate_init(info *info)
{
    node *initFun;
    node *body;
    node *stmtList;
    list *assigns;

    DBUG_ENTER("generate_init");

    assigns = INFO_ASSIGNS(info);
    stmtList = NULL;

    // First of all we need to create a stmtList for the assign vars
    while((assigns = assigns->next)) {

        if (assigns->value == NULL) {
            DBUG_PRINT("HELP", ("%s", assigns->value));
        } else {
            DBUG_PRINT("GVD", ("%s", assigns->value));
        }

        stmtList = TBmakeStmts(
            assigns->value,
            stmtList
        );
    }

    body = TBmakeInnerblock(
                NULL,
                stmtList
           );

    initFun = TBmakeFun(
                global_prefix_none,
                TY_void,
                STRcpy("__init"),
                NULL,
                body,
                TBmakeSymboltable(NULL)
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

    construct_init(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
