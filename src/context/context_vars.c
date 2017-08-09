#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "hash_list.h"

#include "context_vars.h"

struct INFO {
    int nest_lvl;
    hashmap *global;
    hashmap *local;
};

#define INFO_NESTLVL(n)     ((n)->nest_lvl)
#define INFO_GLOBAL(n)      ((n)->global)
#define INFO_LOCAL(n)       ((n)->local)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_NESTLVL(result)    = 0;
    INFO_GLOBAL(result)     = new_map();
    INFO_LOCAL(result)      = new_map();   

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    free(INFO_GLOBAL(info));
    free(INFO_LOCAL(info));

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *CAVscope(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVscope");

    node *function;
    function = SCOPE_FUNS(arg_node);
    
    if (function != NULL) {
        function = TRAVdo(function, arg_info);
    }    

    node *var;
    var = SCOPE_VARS(arg_node);

    if (var != NULL) {
        var = TRAVdo(var, arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *CAVfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVfun");

    DBUG_PRINT("CAV", ("Processing function '%s'", FUN_ID(arg_node)));

    FUN_BODY(arg_node) = TRAVopt(FUN_BODY(arg_node), arg_info);
    FUN_NEXT(arg_node) = TRAVopt(FUN_NEXT(arg_node), arg_info);

    printf("Funny ID = %s", FUN_ID(arg_node));

    DBUG_RETURN(arg_node);
}

node *CAVvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVvar");

    printf("Zo fijn om een var te vinden, zeker als het %s is", VARDEF_ID(arg_node));   

    DBUG_RETURN(arg_node);
}

node *CAVdoContextAnalysisVar(node *syntaxtree)
{
    DBUG_ENTER("CAVdoContextAnalysisVar");

    DBUG_ASSERT((syntaxtree != NULL), "CAVdoContextAnalysisVar called with a empty syntaxtree");

    info *info;
    info = MakeInfo();

    TRAVpush(TR_cav);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info); 

    DBUG_RETURN(syntaxtree);
}
