#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"

#include "context.h"

struct INFO {
    node *currentScope;
    bool checkedFun;
    int externalFuns;
    int externalVars;
};

#define INFO_CURSCOPE(n)        ((n)->currentScope)
#define INFO_CHECKEDFUN(n)      ((n)->checkedFun)
#define INFO_EXTERNALFUNS(n)    ((n)->externalFuns)
#define INFO_EXTERNALVARS(n)    ((n)->externalVars)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_CURSCOPE(result)       = NULL;
    INFO_EXTERNALFUNS(result)   = 0;
    INFO_EXTERNALVARS(result)   = 0;    
    INFO_CHECKEDFUN(result)     = FALSE;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

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
void checkFunction(node *fun, info *arg_info)
{

    if (INFO_CHECKEDFUN(arg_info) == TRUE) {
        printf("Already checked");
        return;
    }

    printf("Not checked yet");
    INFO_CHECKEDFUN(arg_info) = TRUE;

}

node *CAscope(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAscope");
 
    //registerFunctions(SCOPE_FUNS(arg_node), arg_info);
 
    SCOPE_FUNS(arg_node) = TRAVdo(SCOPE_FUNS(arg_node), arg_info);

    //SCOPE_VARS(arg_node) = TRAVdo(SCOPE_VARS(arg_node), arg_info);

    //SCOPE_
 
    printf("En hier hebben we een scopie");
 
    //TRAVdo(arg_node, NULL);

    DBUG_RETURN( arg_node); 
}

node *CAfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAfun");

    checkFunction(arg_node, arg_info);

    printf("Aangenaam, dit is de grappenmaker van het stel %s \n", FUN_ID(arg_node));

    //TRAVdo(arg_node, NULL);

    DBUG_RETURN( arg_node);
}

node *CAdoContextAnalysis(node *syntaxtree)
{
    DBUG_ENTER("Do context analysis");

    info *info;
    info = MakeInfo();

    TRAVpush(TR_ca);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
