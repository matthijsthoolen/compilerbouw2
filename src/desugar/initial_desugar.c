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

#include "initial_desugar.h"

struct INFO {
    bool checkedFunctions;
    int nestLevel;
    list *vardefs;
    hashmap *changeQueue;
    int varCounter;
};

#define INFO_CHECKEDFUNCTIONS(n)    ((n)->checkedFunctions)
#define INFO_NESTLVL(n)             ((n)->nestLevel)
#define INFO_VARDEFS(n)             ((n)->vardefs)
#define INFO_QUEUE(n)               ((n)->changeQueue)
#define INFO_VARCOUNTER(n)          ((n)->varCounter)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_CHECKEDFUNCTIONS(result)   = FALSE;
    INFO_NESTLVL(result)            = 0;
    INFO_VARDEFS(result)            = list_new();
    INFO_QUEUE(result)              = new_map();
    INFO_VARCOUNTER(result)         = 0;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    info = MEMfree(info);

    DBUG_RETURN(info);
}

/**
 * Append the vars in a (inner)block with the given vardefs list
 */
node *append_vars(node *vars, list *vardefs)
{
    node *vardef_list;
    node *tail;

    DBUG_ENTER("append_vars");

    vardef_list = NULL;

    //Create a vardeflist (nodes) from the tmp vardefs list
    while((vardefs = vardefs->next)) {
        vardef_list = TBmakeVardeflist(vardefs->value, vardef_list);
    }

    tail = vardef_list;

    while(VARDEFLIST_NEXT(tail)) {
        tail = VARDEFLIST_NEXT(tail);
    }

    // Append the new list with the 'original'
    VARDEFLIST_NEXT(tail) = vars;

    DBUG_RETURN(vardef_list);
}

node *DSEstmts(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSEstmts");

    STMTS_STMT(arg_node) = TRAVopt(STMTS_STMT(arg_node), arg_info);
    STMTS_NEXT(arg_node) = TRAVopt(STMTS_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *DSEfun(node *arg_node, info *arg_info)
{
    node *body;

    DBUG_ENTER("DSEfun");

    body = FUN_BODY(arg_node);

    if (body != NULL) {
        body = TRAVdo(body, arg_info);
    }

    INFO_CHECKEDFUNCTIONS(arg_info) = TRUE;

    DBUG_RETURN(arg_node);
}

node *DSEinnerblock(node *arg_node, info *arg_info)
{
    node *stmts;

    DBUG_ENTER("DSEblock");

    /**
     * Check stmts
     */
    stmts = INNERBLOCK_STMTS(arg_node);

    if (stmts != NULL) {
        stmts = TRAVdo(stmts, arg_info);
    }

    if (list_length(INFO_VARDEFS(arg_info)) > 0) {
        INNERBLOCK_VARS(arg_node) = append_vars(INNERBLOCK_VARS(arg_node), arg_info->vardefs);
    }

    // Clear the list for the next round
    INFO_VARDEFS(arg_info) = list_new();

    DBUG_RETURN(arg_node);
}

node *DSEfor(node *arg_node, info *arg_info)
{
    node *new_vardef;
    node *for_assign;

    DBUG_ENTER("DSEfor");

    INFO_NESTLVL(arg_info)++;
    INFO_VARCOUNTER(arg_info)++;

    for_assign  = FOR_ASSIGN(arg_node);

    char *old_name = STRcpy(VAR_NAME(ASSIGN_LEFT(for_assign)));

    // Change the name. Based on the nesting LvL and the occurence. 'int i' in first nesting become 'int i--1'
    VAR_NAME(ASSIGN_LEFT(for_assign)) = STRcat(
        STRcat(VAR_NAME(ASSIGN_LEFT(for_assign)),
            STRcat("--",STRitoa(INFO_VARCOUNTER(arg_info)))
        ),
        STRcat("--", STRitoa(INFO_NESTLVL(arg_info)))
    );

    new_vardef = TBmakeVardef(
                    0,
                    TY_int,
                    0,
                    VAR_NAME(ASSIGN_LEFT(for_assign)),
                    NULL,
                    NULL
              );

    // Add the new name and old name to the queue for changing the names in a later stage
    map_push(INFO_QUEUE(arg_info), old_name, STRcpy(VAR_NAME(ASSIGN_LEFT(for_assign))));

    FOR_BLOCK(arg_node) = TRAVopt(FOR_BLOCK(arg_node), arg_info);

    list_reversepush(arg_info->vardefs, new_vardef);

    INFO_NESTLVL(arg_info)--;

    DBUG_RETURN(arg_node);
}

node *DSEvar(node *arg_node, info *arg_info)
{
    char *name;
    char *new_name;

    DBUG_ENTER("DSEvar");

    // Only check nested vars
    if (INFO_NESTLVL(arg_info) > 0) {
        name = VAR_NAME(arg_node);

        if ((new_name = map_get(INFO_QUEUE(arg_info), name)) != NULL) {
            VAR_NAME(arg_node) = new_name;
        }
    }

    DBUG_RETURN(arg_node);
}

node *DSEdoEarlyDesugar(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSdoEarlyDesugar");

    DBUG_ASSERT((syntaxtree != NULL), "DSEdoEarlyDesugar called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_dse);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    FreeInfo(info);

    DBUG_RETURN(syntaxtree);
}
