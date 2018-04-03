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
    bool is_first;
    int const_count;
};

#define INFO_CURSYMBOLTABLE(n) ((n)->symbol_table)
#define INFO_ISFIRST(n)        ((n)->is_first)
#define INFO_CONSTCOUNT(n)     ((n)->const_count)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_CURSYMBOLTABLE(result) = NULL;
    INFO_ISFIRST(result) = TRUE;
    INFO_CONSTCOUNT(result) = 1;

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

    // INFO_CURSYMBOLTABLE(arg_info) = PROGRAM_SYMBOLTABLE(arg_node);

    if (INFO_ISFIRST(arg_info) == TRUE) {
        PROGRAM_ISGLOBAL(arg_node) = TRUE;
    } else {
        PROGRAM_ISGLOBAL(arg_node) = FALSE;
    }

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);
    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    // PROGRAM_SYMBOLTABLE(arg_node) = INFO_CURSYMBOLTABLE(arg_info);

    DBUG_RETURN(arg_node);
}

node *CAfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAfun");

    DBUG_PRINT("CA", ("Processing function '%s'", FUN_ID(arg_node)));

    node *previousScope = INFO_CURSYMBOLTABLE(arg_info);
    node *funSymbolTable = TBmakeSymboltable(NULL);

    FUN_SYMBOLTABLE(arg_node) = funSymbolTable;
    INFO_CURSYMBOLTABLE(arg_info) = FUN_SYMBOLTABLE(arg_node);

    FUN_PARAMS(arg_node) = TRAVopt(FUN_PARAMS(arg_node), arg_info);

    // addToSymboltable(
    //     INFO_CURSYMBOLTABLE(arg_info),
    //     arg_node,
    //     FUN_ID(arg_node),
    //     FUN_RETTY(arg_node),
    //     1 //TODO: something usefull
    // );

    if (FUN_BODY(arg_node)) {
        TRAVopt(FUN_BODY(arg_node), arg_info);
    }

    INFO_CURSYMBOLTABLE(arg_info) = previousScope;

    DBUG_RETURN(arg_node);
}

node *CAvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAvardef");

    DBUG_PRINT("CA", ("Processing vardef '%s'", VARDEF_ID(arg_node)));

    INFO_CONSTCOUNT(arg_info)++;

    node *symbolTableEntry = addToSymboltable(
        INFO_CURSYMBOLTABLE(arg_info),
        arg_node,
        VARDEF_ID(arg_node),
        VARDEF_TY(arg_node),
        INFO_CONSTCOUNT(arg_info),
        0
    );

    VARDEF_SYMBOLTABLEENTRY(arg_node) = symbolTableEntry;

    VARDEF_INIT(arg_node) = TRAVopt(VARDEF_INIT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAvar");

    DBUG_PRINT("CA", ("Processing var '%s'", VAR_NAME(arg_node)));

    DBUG_RETURN(arg_node);
}

node *CAfunparam(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAFunParam");

    DBUG_PRINT("CA", ("Processing funparam '%s'", FUNPARAM_ID(arg_node)));

    node *symbolTableEntry = addToSymboltable(
        INFO_CURSYMBOLTABLE(arg_info),
        arg_node,
        FUNPARAM_ID(arg_node),
        FUNPARAM_TY(arg_node),
        0,
        0
    );

    FUNPARAM_SYMBOLTABLEENTRY(arg_node) = symbolTableEntry;

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
