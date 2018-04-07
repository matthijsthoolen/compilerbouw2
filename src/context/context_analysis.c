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
    node* global_symbol_table;
    bool is_first;
    bool global;
    int const_count;
    int glob_const_count;
    int extern_fun_count;
};

#define INFO_CURSYMBOLTABLE(n)  ((n)->symbol_table)
#define INFO_GLOBSYMBOLTABLE(n) ((n)->global_symbol_table)
#define INFO_ISFIRST(n)         ((n)->is_first)
#define INFO_GLOBAL(n)          ((n)->global)
#define INFO_CONSTCOUNT(n)      ((n)->const_count)
#define INFO_GLOBCONSTCOUNT(n)  ((n)->glob_const_count)
#define INFO_EXTERNFUNCOUNT(n)  ((n)->extern_fun_count)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    INFO_GLOBSYMBOLTABLE(result) = NULL;
    INFO_CURSYMBOLTABLE(result) = NULL;
    INFO_GLOBAL(result) = TRUE;
    INFO_ISFIRST(result) = TRUE;
    INFO_CONSTCOUNT(result) = 0;
    INFO_GLOBCONSTCOUNT(result) = 0;
    INFO_EXTERNFUNCOUNT(result) = 0;

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

    //INFO_CURSYMBOLTABLE(arg_info) = PROGRAM_SYMBOLTABLE(arg_node);

    INFO_GLOBAL(arg_info) = TRUE;

    if (INFO_ISFIRST(arg_info) == TRUE) {
        PROGRAM_ISGLOBAL(arg_node) = TRUE;
        INFO_ISFIRST(arg_info) = FALSE;
        INFO_GLOBSYMBOLTABLE(arg_info) = PROGRAM_SYMBOLTABLE(arg_node);
    } else {
        PROGRAM_ISGLOBAL(arg_node) = FALSE;
    }

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);
    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    // PROGRAM_SYMBOLTABLE(arg_node) = INFO_GLOBSYMBOLTABLE(arg_info);

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
    INFO_GLOBAL(arg_info) = FALSE;

    FUN_PARAMS(arg_node) = TRAVopt(FUN_PARAMS(arg_node), arg_info);

    if (FUN_PREFIX(arg_node) == global_prefix_extern) {
        SYMBOLTABLE_INDEX(funSymbolTable) = INFO_EXTERNFUNCOUNT(arg_info);
        INFO_EXTERNFUNCOUNT(arg_info)++;
    } else if (FUN_BODY(arg_node)) {
        TRAVopt(FUN_BODY(arg_node), arg_info);
    }

    INFO_CURSYMBOLTABLE(arg_info) = previousScope;

    DBUG_RETURN(arg_node);
}

node *CAvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAvardef");

    DBUG_PRINT("CA", ("Processing vardef '%s'", VARDEF_ID(arg_node)));

    if (INFO_GLOBAL(arg_info) == TRUE) {
        DBUG_PRINT("CA", ("Sending global scope"));
    } else {
        DBUG_PRINT("CA", ("Sending local scope"));
    }

    node *symbolTableEntry = addToSymboltable(
        (INFO_GLOBAL(arg_info) == TRUE) ? INFO_GLOBSYMBOLTABLE(arg_info) : INFO_CURSYMBOLTABLE(arg_info),
        arg_node,
        VARDEF_ID(arg_node),
        VARDEF_TY(arg_node),
        (INFO_GLOBAL(arg_info) == TRUE) ? INFO_GLOBCONSTCOUNT(arg_info) : INFO_CONSTCOUNT(arg_info),
        (INFO_GLOBAL(arg_info) == TRUE) ? 0 : 1,
        (INFO_GLOBAL(arg_info) == TRUE) ? SYMBOLTABLE_VARCOUNT(INFO_GLOBSYMBOLTABLE(arg_info)) : SYMBOLTABLE_VARCOUNT(INFO_CURSYMBOLTABLE(arg_info))
    );

    VARDEF_SYMBOLTABLEENTRY(arg_node) = symbolTableEntry;

    if (INFO_GLOBAL(arg_info) == TRUE) {
        INFO_GLOBCONSTCOUNT(arg_info)++;
        // DBUG_PRINT("CA", ("Added vardef %s to the global scope %d", VARDEF_ID(arg_node), SYMBOLTABLE_VARCOUNT(INFO_CURSYMBOLTABLE(arg_info))));
    } else {
        INFO_CONSTCOUNT(arg_info)++;
    }

    VARDEF_INIT(arg_node) = TRAVopt(VARDEF_INIT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CAvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAvar");

    DBUG_PRINT("CA", ("Processing var '%s'", VAR_NAME(arg_node)));

    DBUG_RETURN(arg_node);
}

node *CAint(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAint");

    DBUG_PRINT("CA", ("Processing int '%d'", INT_VALUE(arg_node)));

    if (INFO_GLOBAL(arg_info) == TRUE) {
        INT_ISGLOBAL(arg_node) = TRUE;
    }

    DBUG_RETURN(arg_node);
}

node *CAfloat(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CAfloat");

    DBUG_PRINT("CA", ("Processing var '%f'", FLOAT_VALUE(arg_node)));

    if (INFO_GLOBAL(arg_info) == TRUE) {
        FLOAT_ISGLOBAL(arg_node) = TRUE;
    }

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
        1,
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
