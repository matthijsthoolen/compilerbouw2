/**
 * @file context_type.c
 *
 * Do type checking
 */

#include "context_type.h"

#include "print.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "ctinfo.h"
#include "memory.h"
#include "dbug.h"
#include "str.h"

struct INFO {
    type returnType;
    type funReturnType;
    char * funName;
};

#define INFO_TYPE(n) ((n)->returnType)
#define INFO_FUNTYPE(n) ((n)->funReturnType)
#define INFO_FUNNAME(n) ((n)->funName)

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_TYPE(result) = TY_unknown;
    INFO_FUNTYPE(result) = TY_unknown;

    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *CATCprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCprogram");

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);

    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CATCfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCfun");

    // Still not checking extern function, but now we do check void to make sure nothing is returned
    if (FUN_PREFIX(arg_node) != global_prefix_extern) {
        INFO_FUNTYPE(arg_info) = FUN_RETTY(arg_node);
        INFO_FUNNAME(arg_info) = FUN_ID(arg_node);
    }

    FUN_BODY(arg_node) = TRAVopt(FUN_BODY(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CATCvardeflist(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCvardeflist");

    VARDEFLIST_HEAD(arg_node) = TRAVdo(VARDEFLIST_HEAD(arg_node), arg_info);

    VARDEFLIST_NEXT(arg_node) = TRAVopt(VARDEFLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CATCvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCvardef");

    if (VARDEF_TY(arg_node) == TY_void) {
        CTIerrorLine(
            NODE_LINE(arg_node),
            "Type mismatch for %s. Vardef cant be of type void.",
            VARDEF_ID(arg_node)
        );
    }

    DBUG_RETURN(arg_node);
}

node *CATCreturn(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCreturn");

    if (RETURN_EXPR(arg_node) != NULL) {
        TRAVdo(RETURN_EXPR(arg_node), arg_info);
        RETURN_TY(arg_node) = INFO_TYPE(arg_info);
    } else {
        RETURN_TY(arg_node) = TY_void;
    }

    // Check if the expected return type for this function equals the type of
    // the returned expression
    if (RETURN_TY(arg_node) != INFO_FUNTYPE(arg_info)) {
        CTIerrorLine(
            NODE_LINE(arg_node),
            "function '%s' has return type %s, got %s",
            INFO_FUNNAME(arg_info),
            pretty_print_type(INFO_FUNTYPE(arg_info)),
            pretty_print_type(RETURN_TY(arg_node))
        );
    }

    // Reset info
    INFO_TYPE(arg_info) = TY_unknown;

    DBUG_RETURN(arg_node);
}

node *CATCassign(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCassign");
    type leftType;

    if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_vardef) {
        leftType = VARDEF_TY(VAR_DECL(ASSIGN_LEFT(arg_node)));
    } else if (NODE_TYPE(VAR_DECL(ASSIGN_LEFT(arg_node))) == N_funparam) {
        leftType = FUNPARAM_TY(VAR_DECL(ASSIGN_LEFT(arg_node)));
    }

    TRAVdo(ASSIGN_RIGHT(arg_node), arg_info);

    if (leftType != TY_unknown && INFO_TYPE(arg_info) != TY_unknown && leftType != INFO_TYPE(arg_info)) {
        CTIerror(
            "Row %d: type mismatch for %s. Trying to assign a %s to a variable with type %s",
            NODE_LINE(arg_node),
            VAR_NAME(ASSIGN_LEFT(arg_node)),
            pretty_print_type(INFO_TYPE(arg_info)),
            pretty_print_type(leftType)
        );
    }

    // Reset
    INFO_TYPE(arg_info) = TY_unknown;

    DBUG_RETURN(arg_node);
}

node *CATCwhile(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCwhile");

    WHILE_COND(arg_node) = TRAVdo(WHILE_COND(arg_node), arg_info);

    if (INFO_TYPE(arg_info) != TY_bool) {
        CTIerror(
            "Row %d: the while condition must return a boolean to succeed",
            NODE_LINE(WHILE_COND(arg_node))
        );
    }

    // Check if the while block is empty and warn the user if so
    if (WHILE_BLOCK(arg_node) == NULL) {
        CTIwarn(
            "Row %d: While block is empty. Are you sure this is correct?",
            NODE_LINE(arg_node)
        );
    }

    WHILE_BLOCK(arg_node) = TRAVopt(WHILE_BLOCK(arg_node), arg_info);

    // reset
    INFO_TYPE(arg_info) = TY_unknown;

    DBUG_RETURN(arg_node);
}

node *CATCdowhile(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCdowhile");

    DOWHILE_COND(arg_node) = TRAVdo(DOWHILE_COND(arg_node), arg_info);

    if (INFO_TYPE(arg_info) != TY_bool) {
        CTIerror(
            "Row %d: theDoWhile condition must return a boolean to succeed",
            NODE_LINE(DOWHILE_COND(arg_node))
        );
    }

    // Check if the while block is empty and warn the user if so
    if (DOWHILE_BLOCK(arg_node) == NULL) {
        CTIwarn(
            "Row %d: DoWhile block is empty. Are you sure this is correct?",
            NODE_LINE(arg_node)
        );
    }

    DOWHILE_BLOCK(arg_node) = TRAVopt(DOWHILE_BLOCK(arg_node), arg_info);

    // reset
    INFO_TYPE(arg_info) = TY_unknown;
    DBUG_RETURN(arg_node);
}

node *CATCif(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCif");

    TRAVdo(IF_COND(arg_node), arg_info);

    if (INFO_TYPE(arg_info) != TY_bool) {
        CTIerror(
            "Row %d: the condition in the if statement must return a boolean to succeed",
            NODE_LINE(IF_COND(arg_node))
        );
    }

    // Check the if blocks for type check errors
    if (IF_BLOCKT(arg_node) == NULL) {
        CTIwarn(
            "Row %d: if block is empty. Are you sure this is correct?",
            NODE_LINE(IF_COND(arg_node))
        );
    }

    IF_BLOCKT(arg_node) = TRAVopt(IF_BLOCKT(arg_node), arg_info);
    IF_BLOCKF(arg_node) = TRAVopt(IF_BLOCKF(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *CATCvar(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCvar");

    if (NODE_TYPE(VAR_DECL(arg_node)) == N_vardef) {
        INFO_TYPE(arg_info) = VARDEF_TY(VAR_DECL(arg_node));
    } else if (NODE_TYPE(VAR_DECL(arg_node)) == N_funparam) {
        INFO_TYPE(arg_info) = FUNPARAM_TY(VAR_DECL(arg_node));
    }

    DBUG_RETURN(arg_node);
}


node *CATCmonop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCmonop");

    TRAVdo(MONOP_EXPR(arg_node), arg_info);

    switch(MONOP_OP(arg_node))
    {
        case MO_neg:
            if (INFO_TYPE(arg_info) == TY_bool) {
                CTIerror(
                    "Row '%d': unary minus operation is only possible for boolean types.",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            }
            break;
        case MO_not:
            if (INFO_TYPE(arg_info) != TY_bool) {
                CTIerror(
                    "Row '%d': logical negation is only possible for arithmetic types.",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            }
            break;
        case MO_unknown:
            break;
    }

    DBUG_RETURN(arg_node);
}

/**
 * Check if the binop operation is valid. First get the types of the left hand
 * and right hand side. Then check if the types correspond to the binop
 * operation.
 */
node *CATCbinop(node *arg_node, info *arg_info)
{
    type left;
    type right;

    DBUG_ENTER("CATCbinop");

    TRAVdo(BINOP_LEFT(arg_node), arg_info);
    left = INFO_TYPE(arg_info);

    TRAVdo(BINOP_RIGHT(arg_node), arg_info);
    right = INFO_TYPE(arg_info);

    if (left == TY_unknown || right == TY_unknown) {
        DBUG_PRINT("CATC", ("Failed retrieving binop types"));
        DBUG_RETURN(arg_node);
    }

    switch(BINOP_OP(arg_node)) {
        case BO_lt:
        case BO_le:
        case BO_gt:
        case BO_ge:
            if(left != right || left == TY_bool || right == TY_bool) {
                CTIerror(
                    "Row %d: incorrect binary operator. Only arithmetic types can be compared.",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            } else {
                INFO_TYPE(arg_info) = TY_bool;
            }
            break;
        case BO_add:
        case BO_mul:
            if (left == right) {
                INFO_TYPE(arg_info) = left;
            } else {
                CTIerror(
                    "Row %d: incorrect binary operator.",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            }
            break;
        case BO_mod:
            if (left != TY_int || right != TY_int) {
                CTIerror(
                    "Row %d: incorrect binary operator. Both sides must be integers.",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            } else {
                INFO_TYPE(arg_info) = TY_int;
            }
            break;
        case BO_sub:
        case BO_div:
            if (left != right || left == TY_bool || right == TY_bool) {
                CTIerror(
                    "Row %d: incorrect binary operator.",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            } else {
                INFO_TYPE(arg_info) = left;
            }
            break;
        case BO_eq:
        case BO_ne:
            if (left == right) {
                INFO_TYPE(arg_info) = TY_bool;
            } else {
                CTIerror(
                    "Row %d: incorrect binary operator. Both sides must be of the same type to be compared.",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            }
            break;
        case BO_and:
        case BO_or:
            if (left == TY_bool && right == TY_bool) {
                INFO_TYPE(arg_info) = TY_bool;
            } else {
                CTIerror(
                    "Row %d: incorrect binary operator. Both sides must be boolean expressions",
                    NODE_LINE(arg_node)
                );
                INFO_TYPE(arg_info) = TY_unknown;
            }
            break;
        default:
            CTIerror(
                "Unknown binop operator on line %d",
                NODE_LINE(arg_node)
            );
    }

    DBUG_RETURN(arg_node);
}

node *CATCint(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCint");

    INFO_TYPE(arg_info) = TY_int;

    DBUG_RETURN(arg_node);
}

node *CATCfloat(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCfloat");

    INFO_TYPE(arg_info) = TY_float;

    DBUG_RETURN(arg_node);
}

node *CATCbool(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCbool");

    INFO_TYPE(arg_info) = TY_bool;

    DBUG_RETURN(arg_node);
}

node *CATCcast(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCcast");

    TRAVdo(CAST_EXPR(arg_node), arg_info);

    switch(INFO_TYPE(arg_info))
    {
        case TY_int:
        case TY_bool:
        case TY_float:
            INFO_TYPE(arg_info) = CAST_TY(arg_node);
            break;
        case TY_void:
            CTIerror(
                "Row '%d': can't cast to void",
                NODE_LINE(arg_node)
            );
            INFO_TYPE(arg_info) = TY_unknown;
        default:
            break;
    }

    DBUG_RETURN(arg_node);
}

node *CATCcall(node *arg_node, info *arg_info)
{
    DBUG_ENTER("CATCcall");

    int countArgs;
    int countParams;
    int i;
    node *list;
    node *list2;
    type exprType;
    type paramType;

    countArgs = 0;
    countParams = 0;

    // Count the number of given arguments
    list = CALL_ARGS(arg_node);
    while (list) {
        list = EXPRLIST_NEXT(list);
        countArgs++;
    }

    // Now count the function parameters
    list = FUN_PARAMS(CALL_DECL(arg_node));
    while (list) {
        list = FUNPARAMLIST_NEXT(list);
        countParams++;
    }
    if (countArgs != countParams) {
        INFO_TYPE(arg_info) = TY_unknown;

        CTIerror(
            "Row '%d': %s is called with %d parameters but requires %d.",
            NODE_LINE(arg_node),
            CALL_ID(arg_node),
            countArgs,
            countParams
        );
    }

    // Check if all given arguments are of the correct type
    i = 1;

    list  = CALL_ARGS(arg_node);
    list2 = FUN_PARAMS(CALL_DECL(arg_node));

    while (list) {
        TRAVdo(EXPRLIST_EXPR(list), arg_info);
        exprType    = INFO_TYPE(arg_info);
        paramType   = FUNPARAM_TY(FUNPARAMLIST_PARAM(list2));

        if (exprType != paramType) {
            CTIerror(
                "Row: '%d': incorrect function argument given. Expected %s but got %s for argument nr '%d'",
                NODE_LINE(EXPRLIST_EXPR(list)),
                pretty_print_type(exprType),
                pretty_print_type(paramType),
                i
            );
        }

        list  = EXPRLIST_NEXT(list);
        list2 = FUNPARAMLIST_NEXT(list2);
        i++;
    }

    INFO_TYPE(arg_info) = FUN_RETTY(CALL_DECL(arg_node));

    DBUG_RETURN(arg_node);
}

node *CATCarray(node *arg_node, info *arg_info) {
    DBUG_ENTER("CATCarray");

    DBUG_RETURN(arg_node);
}

char *pretty_print_type(type prt)
{
    if (prt == TY_void) {
        return "void";
    } else if (prt == TY_bool) {
        return "bool";
    } else if (prt == TY_int) {
        return "int";
    } else if (prt == TY_float) {
        return "float";
    } else {
        return "unknown";
    }
}

/**
 * Function for debug purposes only
 */
void print_type_debug(type prt)
{
    if (prt == TY_unknown) {
        printf("Type = unknown");
    } else if (prt == TY_void) {
        printf("Type = void");
    } else if (prt == TY_bool) {
        printf("Type = bool");
    } else if (prt == TY_int) {
        printf("Type = int\n");
    } else if (prt == TY_float) {
        printf("Type = float\n");
    } else {
        printf("Problem...\n");
    }
}

/*
 * Traversal start function
 */
node *CATCdoContextTypeCheck( node *syntaxtree)
{
    info *info;

    DBUG_ENTER("CATCdoContextTypeCheck");

    DBUG_ASSERT((syntaxtree != NULL), "CATCdoContextTypeCheck called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_catc);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
