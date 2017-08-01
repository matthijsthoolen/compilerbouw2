#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "hash_list.h"

#include "context_functions.h"

struct INFO {
    node    *currentScope;
    bool     checkedFun;
    hashmap *funs;
    hashmap *calls;
};

#define INFO_CURSCOPE(n)        ((n)->currentScope)
#define INFO_CHECKEDFUN(n)      ((n)->checkedFun)
#define INFO_FUNS(n)            ((n)->funs)
#define INFO_CALLS(n)           ((n)->calls)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_CURSCOPE(result)   = NULL;
    INFO_FUNS(result)       = new_map();
    INFO_CALLS(result)      = new_map();    
    INFO_CHECKEDFUN(result) = FALSE;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    // Clear the hashmaps
    free(info->funs);
    free(info->calls);

    info = MEMfree(info);

    DBUG_RETURN(info);
}

void registerFunctions(node *functions, info *arg_info)
{
    //printf("Node = %s \n", FUNCTIONS_PREFIX);

    //if (functions != NULL) {
    //    registerFunctions(FUNCTIONS_NEXT(functions), arg_info);
    //    if (NODE_TYPE(FUNCTIONS_DECLARATION(functions)) == N_fun) {
    //        registerFunction(FUNCTIONS_DECLARATION(functions), arg_info);
    //    }
    //}
}

void registerFunction(node *function, info *arg_info)
{
        
}

/**
 * Check a function if not checked already
 */
node *checkFunction(node *fun, info *arg_info)
{
    DBUG_ENTER("checkFunction");

    if (INFO_CHECKEDFUN(arg_info) == TRUE) {
        printf("Already checked");
        DBUG_RETURN(fun);
    }

    if (map_has(INFO_FUNS(arg_info), FUN_ID(fun))) {    
        CTIerror("Function \"%s\" already defined (first defined at %d:%d)", 
                FUN_ID(fun),
                NODE_LINE(fun),
                NODE_COL(fun)
        );

        DBUG_RETURN(fun);
    }

    map_push(INFO_FUNS(arg_info), FUN_ID(fun), fun);

    printf("Not checked yet");
    INFO_CHECKEDFUN(arg_info) = TRUE;

    DBUG_RETURN(fun);
}

node *CAscope(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAscope");

    node *function;

    function = SCOPE_FUNS(arg_node);

    if (SCOPE_FUNS(arg_node) != NULL) { 
        SCOPE_FUNS(arg_node) = TRAVdo(SCOPE_FUNS(arg_node), arg_info);
    }
    //SCOPE_VARS(arg_node) = TRAVdo(SCOPE_VARS(arg_node), arg_info);

    DBUG_RETURN( arg_node); 
}

node *CAfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAfun");

    printf("Going to check %s now", FUN_ID(arg_node));

    if (map_has(INFO_FUNS(arg_info), FUN_ID(arg_node))) {    
        CTIerror("Function \"%s\" already defined (first defined at %d:%d)", 
                FUN_ID(arg_node),
                NODE_LINE(arg_node),
                NODE_COL(arg_node)
        );

        DBUG_RETURN(arg_node);
    }

    map_push(INFO_FUNS(arg_info), FUN_ID(arg_node), arg_node);

    FUN_NEXT(arg_node) = TRAVopt(FUN_NEXT(arg_node), arg_info);

    DBUG_RETURN( arg_node);
}

node *CAcall(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAcall");

    DBUG_RETURN( arg_node);
}

node *CAdoContextAnalysisFun(node *syntaxtree)
{
    DBUG_ENTER("CAdoContextAnalysisFun");

    info *info;
    info = MakeInfo();

    TRAVpush(TR_ca);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
