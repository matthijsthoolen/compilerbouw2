#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "list.h"

#include "vardef_split.h"

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

/**
 * Append the stmts
 */
node *append_stmts(node *stmts, list *assigns)
{
    node *stmts_list;
    node *tail;

    DBUG_ENTER("Append_stmts");

    stmts_list = NULL;

    //Create a stmtlist from the tmp assign list
    while((assigns = assigns->next)) {
        stmts_list = TBmakeStmts(assigns->value, stmts_list);
    }

    tail = stmts_list;

    while(STMTS_NEXT(tail)) {
        tail = STMTS_NEXT(tail);
    }

    STMTS_NEXT(tail) = stmts;

    DBUG_RETURN(stmts_list);
}

node *DSVSfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSVSfun");

    // We only need to traverse the body
    FUN_BODY(arg_node) = TRAVopt(FUN_BODY(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *DSVSinnerblock(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSVSinnerblock");

    INNERBLOCK_VARS(arg_node) = TRAVopt(INNERBLOCK_VARS(arg_node), arg_info);

    if (list_length(INFO_ASSIGNS(arg_info)) > 0) {
        INNERBLOCK_STMTS(arg_node) = append_stmts(
            INNERBLOCK_STMTS(arg_node),
            INFO_ASSIGNS(arg_info)
        );

        INFO_ASSIGNS(arg_info) = list_new();
    }

    DBUG_RETURN(arg_node);
}

node *DSVSvardeflist(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSVSvardeflist");

    VARDEFLIST_HEAD(arg_node) = TRAVopt(VARDEFLIST_HEAD(arg_node), arg_info);
    
    VARDEFLIST_NEXT(arg_node) = TRAVopt(VARDEFLIST_NEXT(arg_node), arg_info);
    DBUG_RETURN(arg_node);
}

node *DSVSvardef(node *arg_node, info *arg_info)
{
    node *assign;

    DBUG_ENTER("DSVSvardef");

    // If the vardef already has a empty init value we dont need to change it
    if (VARDEF_INIT(arg_node) == NULL) {
        DBUG_RETURN(arg_node);
    }

    // Create a new assignment value and add in to a list
    assign = TBmakeAssign(
                TBmakeVar(STRcpy(VARDEF_ID(arg_node))),
                VARDEF_INIT(arg_node)
             );
 
    // Remove the initial value
    VARDEF_INIT(arg_node) = NULL;
 
    list_push(INFO_ASSIGNS(arg_info), assign);

    DBUG_RETURN(arg_node);
}

node *DSVSdoVardefSplit(node *syntaxtree)
{
    DBUG_ENTER("DSVSdoVardefSplit");

    DBUG_ASSERT((syntaxtree != NULL), "DSVSdoVardefSplit called with a empty syntaxtree");

    info *info;
    info = MakeInfo();

    TRAVpush(TR_dsvs);

    syntaxtree = TRAVdo(syntaxtree, info);    

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN(syntaxtree);
}
