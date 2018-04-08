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
    char *split_from;
    hashmap *global;
    hashmap *local;
};

#define INFO_NESTLVL(n)        ((n)->nest_lvl)
#define INFO_GLOBAL(n)         ((n)->global)
#define INFO_LOCAL(n)          ((n)->local)
#define INFO_SPLIT_FROM(n)     ((n)->split_from)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_NESTLVL(result)        = 0;
    INFO_GLOBAL(result)         = new_map();
    INFO_LOCAL(result)          = new_map();
    INFO_SPLIT_FROM(result)     = NULL;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    free(info->global);
    free(info->local);

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *CAVprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVProgram");

    node *declarations = arg_node;

    // Find and register all global variables first
    while (declarations != NULL) {
        if (NODE_TYPE(PROGRAM_HEAD(declarations)) == N_vardef) {
            node *varDef = PROGRAM_HEAD(declarations);
            PROGRAM_HEAD(declarations) = TRAVdo(varDef, arg_info);
        }

        declarations = PROGRAM_NEXT(declarations);
    }

    // Now that we have found all global variables, do it again for the functions
    declarations = arg_node;

    while (declarations != NULL) {
        if (NODE_TYPE(PROGRAM_HEAD(declarations)) != N_vardef) {
            node *fun = PROGRAM_HEAD(declarations);
            PROGRAM_HEAD(declarations) = TRAVdo(fun, arg_info);
        }

        declarations = PROGRAM_NEXT(declarations);
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

    //if global prefix is none (so just local) else add to global list
    if (VARDEF_GLOBAL(arg_node) == FALSE) {

        DBUG_PRINT("CAV", ("Variable %s is local", VARDEF_ID(arg_node)));

        if (map_has(INFO_LOCAL(arg_info), VARDEF_ID(arg_node))) {
            CTIerror("Double declaration of variable \"%s\" (first defined at %d:%d)",
                      VARDEF_ID(arg_node),
                      NODE_LINE(arg_node),
                      NODE_LINE(arg_node));

            DBUG_RETURN(arg_node);
        }

        map_push(INFO_LOCAL(arg_info), VARDEF_ID(arg_node), arg_node);

    } else {

        DBUG_PRINT("CAV", ("Variable %s is global", VARDEF_ID(arg_node)));

        if (map_has(INFO_GLOBAL(arg_info), VARDEF_ID(arg_node))) {
            CTIerror("Double declaration of variable \"%s\" (first defined at %d:%d)",
                      VARDEF_ID(arg_node),
                      NODE_LINE(arg_node),
                      NODE_LINE(arg_node));

            DBUG_RETURN(arg_node);
        }

        map_push(INFO_GLOBAL(arg_info), VARDEF_ID(arg_node), arg_node);

    }

    DBUG_RETURN(arg_node);
}

node *CAVassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVassign");

    ASSIGN_LEFT(arg_node) = TRAVopt(ASSIGN_LEFT( arg_node), arg_info);

    INFO_SPLIT_FROM(arg_info) = ASSIGN_SPLITFROM(arg_node);

    ASSIGN_RIGHT(arg_node) = TRAVdo(ASSIGN_RIGHT( arg_node), arg_info);

    INFO_SPLIT_FROM(arg_info) = NULL;

    DBUG_RETURN(arg_node);
}

node *CAVvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAVvar");

    node *var_decl;

    DBUG_PRINT("CAV", ("Processing var '%s'", VAR_NAME(arg_node)));

    if (VAR_DECL(arg_node) != NULL) {
        DBUG_PRINT("CAV", ("Skipping because this var already has a DECL defined"));
        DBUG_RETURN(arg_node);
    }

    var_decl = map_get(INFO_LOCAL(arg_info), VAR_NAME(arg_node));

    DBUG_PRINT("CAV", ("SPLITFROM = %s", INFO_SPLIT_FROM(arg_info)));

    // To prevent assigning its own value (e.g. int i = i;)
    if (var_decl && STReq(INFO_SPLIT_FROM(arg_info), VARDEF_ID(var_decl))) {
        var_decl = FALSE;
    }

    // Due to the vardef split we must ensure that the declaration is not set to a local var which is
    // defined at a later moment in the original code.
    if (var_decl && NODE_LINE(var_decl) > NODE_LINE(arg_node)) {
        DBUG_PRINT("CAV", ("row decl %d and row init %d", NODE_LINE(var_decl), NODE_LINE(arg_node)));
        var_decl = FALSE;
    }

    if (!var_decl) {
        DBUG_PRINT("CAV", ("var '%s' not defined local, checking global", VAR_NAME(arg_node)));
        var_decl = map_get(INFO_GLOBAL(arg_info), VAR_NAME(arg_node));
    }

    if (!var_decl) {
        CTIerror(
            "Var '%s' on line is not defined in local or global scope.",
            VAR_NAME(arg_node)
        );
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
