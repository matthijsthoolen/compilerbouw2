#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "symbol_table.h"

#include "context_analysis.h"

struct INFO {
    node* symbol_table;
};

#define INFO_CURSYMBOLTABLE(n) ((n)->symbol_table)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_CURSYMBOLTABLE(result) = NULL;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    free(INFO_CURSYMBOLTABLE(info));

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *CAprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAprogram");

    INFO_CURSYMBOLTABLE(arg_info) = PROGRAM_SYMBOLTABLE(arg_node);

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);
    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAfun");

    DBUG_PRINT("CA", ("Processing function '%s'", FUN_ID(arg_node)));

    // node* funSymbolTable = createNewSymbolTable(arg_node);
    //
    // addToSymboltable(funSymbolTable, arg_node);

    FUN_PARAMS(arg_node) = TRAVopt(FUN_PARAMS(arg_node), arg_info);
    FUN_BODY(arg_node)   = TRAVopt(FUN_BODY(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAvardef");

    DBUG_PRINT("CA", ("Processing vardef '%s'", VARDEF_ID(arg_node)));

    VARDEF_INIT(arg_node) = TRAVopt(VARDEF_INIT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAvar");

    DBUG_PRINT("CA", ("Processing var '%s'", VAR_NAME(arg_node)));

    DBUG_RETURN(arg_node);
}

node *CAdoContextAnalysis(node *syntaxtree)
{
    DBUG_ENTER("CAdoContextAnalysis");

    DBUG_ASSERT((syntaxtree != NULL), "CAdoContextAnalysis called with a empty syntaxtree");

    info *info;
    info = MakeInfo();

    TRAVpush(TR_ca);

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN(syntaxtree);
}
