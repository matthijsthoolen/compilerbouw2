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

struct INFO {
    list* constants_list;
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

static FILE *outfile = NULL;

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_CONSTANTSLIST(result) = list_new();

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    list_free(INFO_CONSTANTSLIST(info));

    info = MEMfree(info);

    DBUG_RETURN(info);
}

static void addFloatConstant(info *arg_info, float value)
{
    DBUG_ENTER("addFloatConstant");

    listItem *item = MEMmalloc(sizeof(listItem));

    item->index  = list_length(INFO_CONSTANTSLIST(arg_info));
    item->type   = TY_float;
    item->valFloat = value;

    list_reversepush(INFO_CONSTANTSLIST(arg_info), item);
}

static void addIntConstant(info *arg_info, int value)
{
    DBUG_ENTER("addIntConstant");

    listItem *item = MEMmalloc(sizeof(listItem));

    item->index  = list_length(INFO_CONSTANTSLIST(arg_info));
    item->type   = TY_int;
    item->valInt = value;

    list_reversepush(INFO_CONSTANTSLIST(arg_info), item);
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

    fprintf(outfile, "\n");

    printConstants(arg_info);

    fprintf(outfile, "\n\n");

    DBUG_RETURN(arg_node);
}

node *GBCfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfun");

    // Print some debug information
    fprintf(
        outfile,
        "; function '%s' with %d parameters and %d local vars\n",
        FUN_ID(arg_node),
        SYMBOLTABLE_PARAMCOUNT(FUN_SYMBOLTABLE(arg_node)),
        SYMBOLTABLE_VARCOUNT(FUN_SYMBOLTABLE(arg_node))
    );

    fprintf(outfile, "%s:\n", FUN_ID(arg_node));

    fprintf(outfile, "    esr %d\n", SYMBOLTABLE_VARCOUNT(FUN_SYMBOLTABLE(arg_node)));

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

    TRAVopt(ASSIGN_RIGHT(arg_node), arg_info);

    if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_vardef) {
        symbolTableEntry = VARDEF_SYMBOLTABLEENTRY(VAR_DECL(ASSIGN_LEFT(arg_node)));
    } else if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_funparam) {
        symbolTableEntry = FUNPARAM_SYMBOLTABLEENTRY(VAR_DECL(ASSIGN_LEFT(arg_node)));
    }

    int index = 1;

    // DBUG_PRINT("GBCassign", ("Node type = %s", get_type_name(NODE_TYPE(ASSIGN_RIGHT(arg_node)))));

    fprintf(outfile, "    %sstore %d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), index);

    DBUG_RETURN(arg_node);
}

node *GBCfloat(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfloat");

    addFloatConstant(arg_info, FLOAT_VALUE(arg_node));

    DBUG_RETURN(arg_node);
}

node *GBCint(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCint");

    addIntConstant(arg_info, INT_VALUE(arg_node));

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

    outfile = stdout;

    TRAVpush(TR_gbc);

    syntaxtree = TRAVdo(syntaxtree, info);

    export_assembly(info);

    TRAVpop();

    info = FreeInfo(info);

    outfile = NULL;

    DBUG_RETURN( syntaxtree);
}
