#include "gen_byte_code.h"

#include "print.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "ctinfo.h"
#include "memory.h"
#include "dbug.h"
#include "str.h"
#include "list.h"
#include "globals.h"

struct INFO {
    list* constants_list;
    bool global;
};

typedef struct CONSTANT_LIST_ITEM {
    int index;
    type type;
    union {
        float valFloat;
        int   valInt;
    };
} listItem;

#define INFO_CONSTANTSLIST(n) ((n)->constants_list)
#define INFO_GLOBAL(n) ((n)->global)

static FILE *outfile = NULL;

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_CONSTANTSLIST(result) = list_new();
    INFO_GLOBAL(result) = FALSE;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    list_free(INFO_CONSTANTSLIST(info));

    info = MEMfree(info);

    DBUG_RETURN(info);
}

static int addFloatConstant(info *arg_info, float value)
{
    DBUG_ENTER("addFloatConstant");

    listItem *item;

    list *current = INFO_CONSTANTSLIST(arg_info);

    // If float value is already in the list, return the index
    while((current = current->next)) {
        item = current->value;

        if (item->type == TY_float && item->valFloat == value) {
            DBUG_RETURN(item->index);
        }
        item = NULL;
    }

    item = MEMmalloc(sizeof(listItem));

    item->index  = list_length(INFO_CONSTANTSLIST(arg_info));
    item->type   = TY_float;
    item->valFloat = value;

    list_reversepush(INFO_CONSTANTSLIST(arg_info), item);

    DBUG_RETURN(item->index);
}

static int addIntConstant(info *arg_info, int value)
{
    DBUG_ENTER("addIntConstant");

    listItem *item;

    list *current = INFO_CONSTANTSLIST(arg_info);

    // If int value is already in the list, return the index
    while((current = current->next)) {
        item = current->value;

        if (item->type == TY_int && item->valInt == value) {
            DBUG_RETURN(item->index);
        }
        item = NULL;
    }

    item = MEMmalloc(sizeof(listItem));

    item->index  = list_length(INFO_CONSTANTSLIST(arg_info));
    item->type   = TY_int;
    item->valInt = value;

    list_reversepush(INFO_CONSTANTSLIST(arg_info), item);

    DBUG_RETURN(item->index);
}

static void printConstants(info *arg_info)
{
    DBUG_ENTER("printConstants");

    list *current = INFO_CONSTANTSLIST(arg_info);
    listItem *item;

    while((current = current->next)) {
        item = current->value;

        if (item->type == TY_int) {
            fprintf(outfile, ".const int %d\n", item->valInt);
        } else if (item->type == TY_float) {
            fprintf(outfile, ".const float %f\n", item->valFloat);
        }
    }

    DBUG_VOID_RETURN;
}

char *getShortType(type type) {
    DBUG_ENTER("getShortType");

    char *strType = "";

    switch (type) {
        case TY_bool:
            strType = "b";
            break;
        case TY_float:
            strType = "f";
            break;
        case TY_int:
            strType = "i";
            break;
        case TY_unknown:
            break;
        case TY_void:
            break;
    }

    DBUG_RETURN(strType);
}

node *GBCprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCprogram");

    if (PROGRAM_ISGLOBAL(arg_node) == TRUE) {
        fprintf(outfile, "; %s\n", "HELLO!");
    }

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);

    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    if (PROGRAM_ISGLOBAL(arg_node) == TRUE) {
        fprintf(outfile, "\n");

        printConstants(arg_info);

        fprintf(outfile, "\n\n");
    }

    DBUG_RETURN(arg_node);
}

node *GBCfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfun");

    // Print some debug information
    fprintf(
        outfile,
        "\n; function '%s' with %d parameters and %d local vars\n",
        FUN_ID(arg_node),
        SYMBOLTABLE_PARAMCOUNT(FUN_SYMBOLTABLE(arg_node)),
        SYMBOLTABLE_VARCOUNT(FUN_SYMBOLTABLE(arg_node))
    );

    fprintf(outfile, "%s:\n", FUN_ID(arg_node));

    if (SYMBOLTABLE_VARCOUNT(FUN_SYMBOLTABLE(arg_node)) > 0) {
        fprintf(outfile, "    esr %d\n", SYMBOLTABLE_VARCOUNT(FUN_SYMBOLTABLE(arg_node)));
    }

    if (STReq(FUN_ID(arg_node), "__init")) {
        DBUG_PRINT("GBC", ("__init function, see it as a global function"));
        INFO_GLOBAL(arg_info) = TRUE;
    } else {
        DBUG_PRINT("GBC", ("Not the __init function"));
        INFO_GLOBAL(arg_info) = FALSE;
    }

    TRAVopt(FUN_PARAMS(arg_node), arg_info);

    TRAVopt(FUN_BODY(arg_node), arg_info);

    // End of function
    switch (FUN_RETTY(arg_node)) {
        case TY_void:
            fprintf(outfile, "    return\n");
            break;
        case TY_float:
            fprintf(outfile, "    freturn\n");
            break;
        case TY_int:
            fprintf(outfile, "    ireturn\n");
            break;
        case TY_bool:
            fprintf(outfile, "    breturn\n");
            break;
        default:
            fprintf(outfile, "    return\n");
    }

    DBUG_RETURN(arg_node);
}

node *GBCfunparam(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfunparam");

    DBUG_RETURN(arg_node);
}

node *GBCcall(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfuncall");

    node *symboltable = FUN_SYMBOLTABLE(CALL_DECL(arg_node));

    fprintf(outfile, "    isrg\n");

    CALL_ARGS(arg_node) = TRAVopt(CALL_ARGS(arg_node), arg_info);

    fprintf(outfile, "    jsr %d %s\n", SYMBOLTABLE_PARAMCOUNT(symboltable), FUN_ID(CALL_DECL(arg_node)));

    DBUG_RETURN(arg_node);
}

node *GBCvardeflist(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvardeflist");

    VARDEFLIST_HEAD(arg_node) = TRAVdo(VARDEFLIST_HEAD(arg_node), arg_info);

    VARDEFLIST_NEXT(arg_node) = TRAVopt(VARDEFLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *GBCvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvardef");

    // nothing to do here
    if (VARDEF_INIT(arg_node) == NULL) {
        DBUG_RETURN(arg_node);
    }

    // if (VARDEF_TY(arg_node) == TY_int) {
    //     addIntConstant(arg_info, INT_VALUE(VARDEF_INIT(arg_node)));
    // } else if (VARDEF_TY(arg_node) == TY_float) {
    //     addFloatConstant(arg_info, FLOAT_VALUE(VARDEF_INIT(arg_node)));
    // }

    DBUG_RETURN(arg_node);
}

node *GBCassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCassign");

    node *symbolTableEntry;
    char *global = "";

    if (NODE_TYPE(ASSIGN_RIGHT(arg_node)) == N_var && NODE_TYPE(ASSIGN_LEFT(arg_node)) == N_var) {
        if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_vardef) {
            symbolTableEntry = VARDEF_SYMBOLTABLEENTRY(VAR_DECL(ASSIGN_RIGHT(arg_node)));
        } else if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_funparam) {
            symbolTableEntry = FUNPARAM_SYMBOLTABLEENTRY(VAR_DECL(ASSIGN_RIGHT(arg_node)));
        }

        fprintf(outfile, "    %sload_%d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), SYMBOLTABLEENTRY_INDEX(symbolTableEntry));
    } else {
        TRAVopt(ASSIGN_RIGHT(arg_node), arg_info);
    }

    if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_vardef) {
        symbolTableEntry = VARDEF_SYMBOLTABLEENTRY(VAR_DECL(ASSIGN_LEFT(arg_node)));
    } else if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_funparam) {
        symbolTableEntry = FUNPARAM_SYMBOLTABLEENTRY(VAR_DECL(ASSIGN_LEFT(arg_node)));
    }

    if (SYMBOLTABLEENTRY_NESTINGLVL(symbolTableEntry) == 0) {
        global = "g";
    }

    // DBUG_PRINT("GBCassign", ("Node type = %s", get_type_name(NODE_TYPE(ASSIGN_RIGHT(arg_node)))));

    fprintf(outfile, "    %sstore%s %d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), global, SYMBOLTABLEENTRY_INDEX(symbolTableEntry));

    DBUG_RETURN(arg_node);
}

node *GBCvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvar");

    node *symbolTableEntry = VARDEF_SYMBOLTABLEENTRY(VAR_DECL(arg_node));

    char *scopeStr = (SYMBOLTABLEENTRY_NESTINGLVL(symbolTableEntry) == 0) ? "g" : "c";
    int index = SYMBOLTABLEENTRY_INDEX(symbolTableEntry);;

    DBUG_PRINT("GBC", ("%s", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry))));

    if (index < 4 && SYMBOLTABLEENTRY_NESTINGLVL(symbolTableEntry) != 0) {
        fprintf(outfile, "    %sload_%d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), index);
    } else {
        fprintf(outfile, "    %sload%s %d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), scopeStr, index);
    }

    DBUG_RETURN(arg_node);
}

node *GBCfloat(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfloat");

    char *scopeStr = (FLOAT_ISGLOBAL(arg_node) == TRUE) ? "g" : "c";

    if (FLOAT_VALUE(arg_node) == 0.0 || FLOAT_VALUE(arg_node) == 1.0) {
        fprintf(outfile, "    fload%s_%d\n", scopeStr, (int)FLOAT_VALUE(arg_node));
        DBUG_RETURN(arg_node);
    }

    FLOAT_CONSTINDEX(arg_node) = addFloatConstant(arg_info, FLOAT_VALUE(arg_node));
    fprintf(outfile, "    fload%s %d\n", scopeStr, list_length(INFO_CONSTANTSLIST(arg_info)) - 1);

    DBUG_RETURN(arg_node);
}

node *GBCint(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCint");

    DBUG_PRINT("GBC", ("Checking int %i", INT_VALUE(arg_node)));

    char *scopeStr = (INT_ISGLOBAL(arg_node) == TRUE && INFO_GLOBAL(arg_info) == FALSE) ? "g" : "c";

    switch(INT_VALUE(arg_node)) {
        case -1:
            fprintf(outfile, "    iload%s_m1\n", scopeStr);
            break;
        case 0:
        case 1:
            fprintf(outfile, "    iload%s_%d\n", scopeStr, INT_VALUE(arg_node));
            break;
        default:
            INT_CONSTINDEX(arg_node) = addIntConstant(arg_info, INT_VALUE(arg_node));
            fprintf(outfile, "    iload%s %d\n", scopeStr, list_length(INFO_CONSTANTSLIST(arg_info)) - 1);
    }

    DBUG_RETURN(arg_node);
}

node *GBCbool(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCint");

    if (BOOL_VALUE(arg_node)) {
        fprintf(outfile, "    bloadc_t\n");
    } else {
        fprintf(outfile, "    bloadc_f\n");
    }

    DBUG_RETURN(arg_node);
}

node *GBCinnerblock(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCinnerblock");

    TRAVopt(INNERBLOCK_VARS(arg_node), arg_info);
    TRAVopt(INNERBLOCK_STMTS(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

void export_assembly(info *info) {
}


/*
 * Traversal start function
 */

node *GBCdoGenByteCode( node *syntaxtree)
{
    info *info;

    DBUG_ENTER("GBCdoGenByteCode");

    DBUG_ASSERT((syntaxtree != NULL), "GBCdoGenByteCode called with empty syntaxtree");

    info = MakeInfo();

    if (global.outfile) {
        outfile = fopen(global.outfile, "w+");
    } else {
        outfile = stdout;
    }

    TRAVpush(TR_gbc);

    syntaxtree = TRAVdo(syntaxtree, info);

    export_assembly(info);

    TRAVpop();

    info = FreeInfo(info);

    if (global.outfile) {
        fclose(outfile);
    }

    outfile = NULL;

    DBUG_RETURN( syntaxtree);
}
