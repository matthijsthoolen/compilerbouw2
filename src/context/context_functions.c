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
    hashmap *funs;
    hashmap *calls;
};

#define INFO_CURSCOPE(n)        ((n)->currentScope)
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

node *CAfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAfun");

    DBUG_PRINT("CA", ("Processing function definition '%s'", FUN_ID(arg_node)));

    if (map_has(INFO_FUNS(arg_info), FUN_ID(arg_node))) {    
        CTIerror("Function \"%s\" already defined (first defined at %d:%d)", 
                FUN_ID(arg_node),
                NODE_LINE(arg_node),
                NODE_COL(arg_node)
        );

        DBUG_RETURN(arg_node);
    }

    map_push(INFO_FUNS(arg_info), FUN_ID(arg_node), arg_node); 

    FUN_BODY(arg_node) = TRAVopt(FUN_BODY(arg_node), arg_info);

    DBUG_RETURN( arg_node);
}

node *CAcall(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAcall");
 
    char *name = VAR_NAME(CALL_ID(arg_node));
    
    DBUG_PRINT("CA", ("Check if function %s is declared.", name)); 
    
    /* Save all the Function Call arguments given */
    CALL_ARGS(arg_node) = TRAVopt(CALL_ARGS(arg_node), arg_info);

    /* Check if function is already defined. If so, we dont need to add it to the queue */
    node *fun = map_get(INFO_FUNS(arg_info), name);

    if (fun) {
        VAR_DECL(CALL_ID(arg_node)) = fun;
    } else {
        DBUG_PRINT("CA", ("Function %s isnt declared yet. Add call to queue", name)); 
        map_push(INFO_CALLS(arg_info), name, arg_node);
    }

    DBUG_RETURN( arg_node);
}

/**
 * Check if all function calls have a callable function in the same scope
 */
void check_fun_calls(info *arg_info) 
{
    hashmap *tmp;

    DBUG_PRINT("CA", ("Check function call queue"));
 
    while(!map_is_empty(INFO_CALLS(arg_info))) {
        tmp = map_pop_reverse(INFO_CALLS(arg_info));

        node *fun = map_get(INFO_FUNS(arg_info), tmp->key);

        if (fun) {
            VAR_DECL(CALL_ID((node *)tmp->value)) = fun;
        } else {
            CTIerror("On line %d\nundefined function '%s'", NODE_LINE((node *)tmp->value), (char *)tmp->key);
        }
    }
}

node *CAdoContextAnalysisFun(node *syntaxtree)
{
    DBUG_ENTER("CAdoContextAnalysisFun");

    info *info;
    info = MakeInfo();

    TRAVpush(TR_ca);

    syntaxtree = TRAVdo( syntaxtree, info);

    check_fun_calls(info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
