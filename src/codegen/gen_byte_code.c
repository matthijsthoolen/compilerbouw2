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
#include "node_helper.h"

struct INFO {
    list* constants_list;
    list* globals_list;
    int labels_if;
    int labels_while;
    int labels_dowhile;
    int labels_for;
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
#define INFO_GLOBALSLIST(n) ((n)->globals_list)
#define INFO_LABELS_IF(n) ((n)->labels_if)
#define INFO_LABELS_WHILE(n) ((n)->labels_while)
#define INFO_LABELS_DOWHILE(n) ((n)->labels_dowhile)
#define INFO_LABELS_FOR(n) ((n)->labels_for)
#define INFO_GLOBAL(n) ((n)->global)

static FILE *outfile = NULL;

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_CONSTANTSLIST(result)  = list_new();
    INFO_GLOBALSLIST(result)   = list_new();
    INFO_LABELS_IF(result)      = 0;
    INFO_LABELS_WHILE(result)   = 0;
    INFO_LABELS_DOWHILE(result) = 0;
    INFO_LABELS_FOR(result)     = 0;
    INFO_GLOBAL(result)         = FALSE;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    list_free(INFO_CONSTANTSLIST(info));
    list_free(INFO_GLOBALSLIST(info));

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

static void printGlobalVardefs(info *arg_info)
{
    DBUG_ENTER("printConstants");

    list *current = INFO_GLOBALSLIST(arg_info);
    listItem *item;

    while((current = current->next)) {
        item = current->value;

        fprintf(outfile, ".global %s\n", get_type_name(item->type));
    }

    DBUG_VOID_RETURN;
}

node *GBCprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCprogram");

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);

    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    if (PROGRAM_ISGLOBAL(arg_node) == TRUE) {
        fprintf(outfile, "\n ; constants: \n");

        printConstants(arg_info);

        fprintf(outfile, "\n ; export functions:\n");

        node *declarations = arg_node;
        node *fun;

        while (declarations != NULL) {
            if (NODE_TYPE(PROGRAM_HEAD(declarations)) == N_fun && FUN_PREFIX(PROGRAM_HEAD(declarations)) == global_prefix_export) {
                fun = PROGRAM_HEAD(declarations);

                DBUG_PRINT("GBC", ("Checking function %s %d", FUN_ID(fun), FUN_PREFIX(fun)));
                fprintf(outfile, ".exportfun \"%s\" %s %s\n" , FUN_ID(fun), get_type_name(FUN_RETTY(fun)), FUN_ID(fun));
            }

            declarations = PROGRAM_NEXT(declarations);
        }

        fprintf(outfile, "\n ; global vardefs\n");

        printGlobalVardefs(arg_info);

        fprintf(outfile, "\n ; import functions:\n");

        declarations = arg_node;

        while (declarations != NULL) {
            if (NODE_TYPE(PROGRAM_HEAD(declarations)) == N_fun && (FUN_PREFIX(PROGRAM_HEAD(declarations)) == global_prefix_extern)) {
                fun = PROGRAM_HEAD(declarations);

                DBUG_PRINT("GBC", ("Checking function %s %d", FUN_ID(fun), FUN_PREFIX(fun)));
                fprintf(outfile, ".importfun \"%s\" %s" , FUN_ID(fun), get_type_name(FUN_RETTY(fun)));

                node *paramList = FUN_PARAMS(fun);

                while (paramList != NULL) {
                    fprintf(outfile, " %s", get_type_name(FUNPARAM_TY(FUNPARAMLIST_PARAM(paramList))));

                    paramList = FUNPARAMLIST_NEXT(paramList);
                }

                fprintf(outfile, "\n");
            }

            declarations = PROGRAM_NEXT(declarations);
        }

        fprintf(outfile, "\n\n");
    }

    DBUG_RETURN(arg_node);
}

node *GBCfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfun");

    if (FUN_PREFIX(arg_node) == global_prefix_extern) {
        DBUG_RETURN(arg_node);
    }

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

    if (FUN_RETTY(arg_node) == TY_void) {
        fprintf(outfile, "    return\n");
    }

    DBUG_RETURN(arg_node);
}

node *GBCreturn(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCreturn");

    if (RETURN_EXPR(arg_node) == NULL) {
        fprintf(outfile, "    return\n");
        DBUG_RETURN(arg_node);
    }

    TRAVdo(RETURN_EXPR(arg_node), arg_info);
    fprintf(outfile, "    %sreturn\n", getShortNodeType(RETURN_EXPR(arg_node)));

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

    if (FUN_PREFIX(CALL_DECL(arg_node)) == global_prefix_extern) {
        fprintf(outfile, "    jsre %d\n", SYMBOLTABLE_INDEX(FUN_SYMBOLTABLE(CALL_DECL(arg_node))));
    } else {
        fprintf(outfile, "    jsr %d %s\n", SYMBOLTABLE_PARAMCOUNT(symboltable), FUN_ID(CALL_DECL(arg_node)));
    }

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

    if (INFO_GLOBAL(arg_info) == TRUE) {
        listItem *item = MEMmalloc(sizeof(listItem));

        item->index  = list_length(INFO_CONSTANTSLIST(arg_info));
        item->type   = VARDEF_TY(arg_node);

        list_reversepush(INFO_GLOBALSLIST(arg_info), item);
    }

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

        if (SYMBOLTABLEENTRY_NESTINGLVL(symbolTableEntry) != 0 && SYMBOLTABLEENTRY_INDEX(symbolTableEntry) < 4) {
            fprintf(outfile, "    %sload_%d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), SYMBOLTABLEENTRY_VARINDEX(symbolTableEntry));
        } else if (SYMBOLTABLEENTRY_NESTINGLVL(symbolTableEntry) == 0) {
            fprintf(outfile, "    %sloadg %d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), SYMBOLTABLEENTRY_VARINDEX(symbolTableEntry));
        } else {
            fprintf(outfile, "    %sload %d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), SYMBOLTABLEENTRY_VARINDEX(symbolTableEntry));
        }
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

    fprintf(outfile, "    %sstore%s %d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), global, SYMBOLTABLEENTRY_VARINDEX(symbolTableEntry));

    DBUG_RETURN(arg_node);
}

node *GBCvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvar");

    node *symbolTableEntry;

    if (NODE_TYPE(VAR_DECL(arg_node)) == N_vardef) {
        symbolTableEntry = VARDEF_SYMBOLTABLEENTRY(VAR_DECL(arg_node));
    } else if (NODE_TYPE(VAR_DECL(arg_node)) == N_funparam) {
        symbolTableEntry = FUNPARAM_SYMBOLTABLEENTRY(VAR_DECL(arg_node));
    }

    char *scopeStr = (SYMBOLTABLEENTRY_NESTINGLVL(symbolTableEntry) == 0) ? "g" : "c";

    int index = SYMBOLTABLEENTRY_VARINDEX(symbolTableEntry);
    DBUG_PRINT("GBC", ("%s", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry))));

    if (index < 4 && SYMBOLTABLEENTRY_NESTINGLVL(symbolTableEntry) != 0) {
        fprintf(outfile, "    %sload_%d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), index);
    } else {
        fprintf(outfile, "    %sload%s %d\n", getShortType(SYMBOLTABLEENTRY_TYPE(symbolTableEntry)), scopeStr, index);
    }

    DBUG_RETURN(arg_node);
}

node *GBCcast(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCcast");

    TRAVdo(CAST_EXPR(arg_node), arg_info);

    if (getNodeType(CAST_EXPR(arg_node)) != TY_bool && CAST_TY(arg_node) != TY_bool) {
        fprintf(outfile, "    %s2%s\n", getShortNodeType(CAST_EXPR(arg_node)), getShortType(CAST_TY(arg_node)));
    }

    DBUG_RETURN(arg_node);
}

node *GBCmonop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCbinop");

    TRAVdo(MONOP_EXPR(arg_node), arg_info);

    fprintf(
        outfile,
        "    %s%s\n",
        getShortNodeType(arg_node),
        MONOP_OP(arg_node) == MO_neg ? "neg" : "not"
    );

    DBUG_RETURN(arg_node);
}

node *GBCbinop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCbinop");

    TRAVdo(BINOP_LEFT(arg_node), arg_info);
    TRAVdo(BINOP_RIGHT(arg_node), arg_info);

    fprintf(outfile, "    %s%s\n", getShortNodeType(BINOP_LEFT(arg_node)), get_binop_string(arg_node));

    DBUG_RETURN(arg_node);
}

node *GBCternop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCternop");

    int i = INFO_LABELS_IF(arg_info)++;

    TRAVdo(TERNOP_COND(arg_node), arg_info);
    fprintf(outfile, "    branch_f %d_ternop_else\n", i);

    TRAVdo(TERNOP_THEN(arg_node), arg_info);
    fprintf(outfile, "    jump %d_ternop_end\n", i);
    fprintf(outfile, "%d_ternop_else:\n", i);

    TRAVdo(TERNOP_ELSE(arg_node), arg_info);
    fprintf(outfile, "%d_ternop_end:\n", i);

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

node *GBCif(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCif");

    if (IF_BLOCKT(arg_node) != NULL || IF_BLOCKF(arg_node) != NULL) {
        TRAVdo(IF_COND(arg_node), arg_info);
        int i = INFO_LABELS_IF(arg_info)++;

        // Only a true block,
        if (IF_BLOCKF(arg_node) == NULL && IF_BLOCKT(arg_node) != NULL) {
            fprintf(outfile, "    branch_f %d_end\n", i);
            TRAVopt(IF_BLOCKT(arg_node), arg_info);
        } else if (IF_BLOCKF(arg_node) != NULL && IF_BLOCKT(arg_node) != NULL){
            fprintf(outfile, "    branch_f %d_else\n", i);
            TRAVopt(IF_BLOCKT(arg_node), arg_info);
            fprintf(outfile, "    jump %d_end\n", i);
            fprintf(outfile, "%d_else:\n", i);
            TRAVopt(IF_BLOCKF(arg_node), arg_info);
        } else if (IF_BLOCKF(arg_node) != NULL && IF_BLOCKT(arg_node) == NULL) {
            fprintf(outfile, "    branch_t %d_end\n", i);
            TRAVopt(IF_BLOCKF(arg_node), arg_info);
        }

        fprintf(outfile, "%d_end:\n", i);
    }

    DBUG_RETURN(arg_node);
}

node *GBCwhile(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCwhile");

    // If while block is empty we don't want to print it and we can exit.
    if (WHILE_BLOCK(arg_node) == NULL) {
        DBUG_RETURN(arg_node);
    }

    int i = INFO_LABELS_WHILE(arg_info)++;
    fprintf(outfile, "%d_while:\n", i);
    TRAVdo(WHILE_COND(arg_node), arg_info);
    fprintf(outfile, "    branch_f %d_while_end\n", i);

    TRAVopt(WHILE_BLOCK(arg_node), arg_info);

    fprintf(outfile, "    jump %d_while\n", i);
    fprintf(outfile, "%d_while_end:\n", i);

    DBUG_RETURN(arg_node);
}

node *GBCdowhile(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCdowhile");

    // If dowhile block is empty we don't want to print it and we can exit.
    if (DOWHILE_BLOCK(arg_node) == NULL) {
        DBUG_RETURN(arg_node);
    }

    int i = INFO_LABELS_DOWHILE(arg_info)++;
    fprintf(outfile, "%d_dowhile:\n", i);

    TRAVopt(DOWHILE_BLOCK(arg_node), arg_info);
    TRAVdo(DOWHILE_COND(arg_node), arg_info);

    fprintf(outfile, "    branch_t %d_dowhile\n", i);

    DBUG_RETURN(arg_node);
}

node *GBCfor(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCwhile");

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
