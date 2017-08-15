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

node *CAVblock(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVblock");
 
    node *var;
    var = BLOCK_VARS(arg_node);

    if (var != NULL) {
        var = TRAVdo(var, arg_info);
    }

    node *function;
    function = BLOCK_FUNS(arg_node);
    
    if (function != NULL) {
        function = TRAVdo(function, arg_info);
    }    

    DBUG_RETURN(arg_node);
}

node *CAVfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVfun");

    DBUG_PRINT("CAV", ("Processing function '%s'", FUN_ID(arg_node)));
    
    FUN_PARAMS(arg_node) = TRAVopt(FUN_PARAMS(arg_node), arg_info);

    FUN_BODY(arg_node) = TRAVopt(FUN_BODY(arg_node), arg_info);

    // Clear Local Info
    map_free(INFO_LOCAL(arg_info));
    INFO_LOCAL(arg_info) = new_map();

    FUN_NEXT(arg_node) = TRAVopt(FUN_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAVfunparamlist(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVfunparamlist");

    FUNPARAMLIST_PARAM(arg_node) = TRAVdo(FUNPARAMLIST_PARAM(arg_node), arg_info);

    FUNPARAMLIST_NEXT(arg_node) = TRAVopt(FUNPARAMLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAVfunparam(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVfunparam");
    
    DBUG_PRINT("CAV", ("Processing function parameters, now checking: '%s'", FUNPARAM_ID(arg_node)));

        
    if (map_has(INFO_LOCAL(arg_info), FUNPARAM_ID(arg_node))) {
        CTIerror("Double declaration of variable \"%s\" (first defined at %d:%d)",
                  FUNPARAM_ID(arg_node),
                  NODE_LINE(arg_node),
                  NODE_LINE(arg_node));

        DBUG_RETURN(arg_node);
    }

    map_push(INFO_LOCAL(arg_info), FUNPARAM_ID(arg_node), arg_node);

    DBUG_RETURN(arg_node);
}

node *CAVvardeflist(node *arg_node, info *arg_info) 
{
    DBUG_ENTER("CAVvardeflist");

    VARDEFLIST_HEAD(arg_node) = TRAVopt(VARDEFLIST_HEAD(arg_node), arg_info);

    VARDEFLIST_NEXT(arg_node) = TRAVopt(VARDEFLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAVvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVvardef");

    DBUG_PRINT("CAV", ("Processing variable definition '%s'", VARDEF_ID(arg_node)));

    if (map_has(INFO_LOCAL(arg_info), VARDEF_ID(arg_node))) {
        CTIerror("Double declaration of variable \"%s\" (first defined at %d:%d)",
                  VARDEF_ID(arg_node),
                  NODE_LINE(arg_node),
                  NODE_LINE(arg_node));

        DBUG_RETURN(arg_node);
    }

    map_push(INFO_LOCAL(arg_info), VARDEF_ID(arg_node), arg_node);

    DBUG_RETURN(arg_node);
}

node *CAVvar(node *arg_node, info *arg_info)
{
    node *var_decl;

    DBUG_ENTER("CAVvar");

    DBUG_PRINT("CAV", ("Processing var '%s'", VAR_NAME(arg_node)));

    var_decl = map_get(INFO_LOCAL(arg_info), VAR_NAME(arg_node));

    if (!var_decl) {
        DBUG_PRINT("CAV", ("var '%s' not defined local, checking global", VAR_NAME(arg_node)));
        var_decl = map_get(INFO_GLOBAL(arg_info), VAR_NAME(arg_node));
    }

    if (!var_decl) {
        CTIerror("Var '%s' is not defined in local or global scope.",
                 VAR_NAME(arg_node));
        DBUG_RETURN(arg_node);
    }

    VAR_DECL(arg_node) = var_decl;
        
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
