#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"

#include "initial_desugar.h"

struct INFO {
    bool checkedFunctions;
    int nestLevel;
};

#define INFO_CHECKEDFUNCTIONS(n) ((n)->checkedFunctions)
#define INFO_NESTLVL(n) ((n)->nestLevel)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_CHECKEDFUNCTIONS(result) = FALSE;
    INFO_NESTLVL(result) = 0;
    
    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *DSEblock(node *arg_node, info *arg_info) {

    node *function;
    node *var;

    DBUG_ENTER("DSEblock");

    /**
     * Check vars
     */

    var = BLOCK_VARS(arg_node);

    if (var != NULL) {
        var = TRAVdo(var, arg_info);
    }

    /**
     * Check functions, but do it only once
     */
    function = BLOCK_FUNS(arg_node);

    if (function != NULL) {
        function = TRAVdo(function, arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *DSEinnerblock(node *arg_node, info *arg_info) {

    node *var;
    node *stmts;

    DBUG_ENTER("DSEblock");

    /**
     * Check vars
     */

    var = INNERBLOCK_VARS(arg_node);

    if (var != NULL) {
        var = TRAVdo(var, arg_info);
    }


    /**
     * Check stmts
     */

    stmts = INNERBLOCK_STMTS(arg_node);
    
    if (stmts != NULL) {
        stmts = TRAVdo(stmts, arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *DSEfun(node *arg_node, info *arg_info) {
    node *body;

    DBUG_ENTER("DSEfun");
   
    body = FUN_BODY(arg_node);
    
    if (body != NULL) {
        body = TRAVdo(body, arg_info);
    }    

    FUN_NEXT(arg_node) = TRAVopt(FUN_NEXT(arg_node), arg_info);

    INFO_CHECKEDFUNCTIONS(arg_info) = TRUE;
 
    DBUG_RETURN(arg_node);
}

node *DSEwhile(node *arg_node, info *arg_info) {
    DBUG_ENTER("DSEwhile");

    DBUG_RETURN(arg_node);
}

node *DSEfor(node *arg_node, info *arg_info) {
    node *new_block;
    node *for_step;

    DBUG_ENTER("DSEfor");

    

    new_block = TBmakeInnerblock(NULL, NULL);
    
    if (FOR_STEP(arg_node) != NULL) { 
    }

    DBUG_RETURN(arg_node);
}

node *DSEdowhile(node *arg_node, info *arg_info) {
    DBUG_ENTER("DSEdowhile");

    DBUG_RETURN(arg_node);
}

node *DSEstmts(node *arg_node, info *arg_info) {
    DBUG_ENTER("DSEstmts");

    DBUG_RETURN(arg_node);
}

node *DSEdoEarlyDesugar(node *syntaxtree) {
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
