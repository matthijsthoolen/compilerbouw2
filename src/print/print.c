/**
 * @file print.c
 *
 * Functions needed by print traversal.
 *
 */

/**
 * @defgroup print Print Functions.
 *
 * Functions needed by print traversal.
 *
 * @{
 */


#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"


/*
 * INFO structure
 */
struct INFO {
    bool firsterror;
    int indent;
    bool ate_newline;
    bool no_semicolon;
};

#define INFO_FIRSTERROR(n)      ((n)->firsterror)
#define INFO_INDENT(n)          ((n)->indent)
#define INFO_IS_NEW_LINE(n)     ((n)->ate_newline)
#define INFO_NO_SEMICOLON(n)    ((n)->no_semicolon)

#define INDENT_NEWLINE(n) INFO_IS_NEW_LINE(n) = TRUE;
#define INDENT_INCREASE(n) INFO_INDENT(n) += 4; INDENT_NEWLINE(n)
#define INDENT_DECREASE(n) INFO_INDENT(n) -= 4; INDENT_NEWLINE(n)

static info *MakeInfo() {
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));

    INFO_FIRSTERROR(result)     = FALSE;
    INFO_INDENT(result)         = 0;
    INFO_IS_NEW_LINE(result)    = FALSE;
    INFO_NO_SEMICOLON(result)   = FALSE;

    DBUG_RETURN(result);
}


static info *FreeInfo( info *info) {
    DBUG_ENTER("FreeInfo");

    info = MEMfree( info);

    DBUG_RETURN(info);
}

static void indent(info *info) {
    if (!info->ate_newline) {
        return;
    }
    info->ate_newline = 0;
    int n = info->indent;
    for (int i = 0; i < n; i++) {
        printf(" ");
    }
}

static void newline(info *info) {
    info->ate_newline = 1;
}


static void print_blocklike(node **stmts, info *arg_info) {
    indent(arg_info);
    printf("{");
    if (*stmts != NULL) {
        printf("\n"); newline(arg_info);
        INDENT_INCREASE(arg_info);
        *stmts = TRAVdo(*stmts, arg_info);
        INDENT_DECREASE(arg_info);
    }
    indent(arg_info);
    printf("}");
}

/** <!--******************************************************************-->
 *
 * @fn PRTstmts
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTstmts (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTstmts");

  STMTS_STMT( arg_node) = TRAVdo( STMTS_STMT( arg_node), arg_info);

  STMTS_NEXT( arg_node) = TRAVopt( STMTS_NEXT( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTassign
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTassign (node * arg_node, info * arg_info)
{
    DBUG_ENTER ("PRTassign");

    indent(arg_info);

    if (ASSIGN_LEFT( arg_node) != NULL) {
        ASSIGN_LEFT( arg_node) = TRAVdo( ASSIGN_LEFT( arg_node), arg_info);
        printf( " = ");
    }

    ASSIGN_RIGHT( arg_node) = TRAVdo( ASSIGN_RIGHT( arg_node), arg_info);

    if (INFO_NO_SEMICOLON(arg_info) == FALSE) {
        printf(";\n");
        newline(arg_info);
    }

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTbinop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbinop (node * arg_node, info * arg_info)
{
  char *tmp;

  DBUG_ENTER ("PRTbinop");

  printf( "( ");

  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);

  switch (BINOP_OP( arg_node)) {
    case BO_add:
      tmp = "+";
      break;
    case BO_sub:
      tmp = "-";
      break;
    case BO_mul:
      tmp = "*";
      break;
    case BO_div:
      tmp = "/";
      break;
    case BO_mod:
      tmp = "%";
      break;
    case BO_lt:
      tmp = "<";
      break;
    case BO_le:
      tmp = "<=";
      break;
    case BO_gt:
      tmp = ">";
      break;
    case BO_ge:
      tmp = ">=";
      break;
    case BO_eq:
      tmp = "==";
      break;
    case BO_ne:
      tmp = "!=";
      break;
    case BO_or:
      tmp = "||";
      break;
    case BO_and:
      tmp = "&&";
      break;
    case BO_unknown:
      DBUG_ASSERT( 0, "unknown binop detected!");
  }

  printf( " %s ", tmp);

  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

  printf( ")");

  DBUG_RETURN (arg_node);
}

node *PRTmonop (node * arg_node, info * arg_info)
{
    char *tmp;

    DBUG_ENTER ("PRTmonop");

    printf( "( ");

    switch (MONOP_OP( arg_node)) {
      case MO_neg:
        tmp = "-";
        break;
      case MO_not:
        tmp = "!";
        break;
      case MO_unknown:
        DBUG_ASSERT( 0, "unknown monop detected!");
    }

    printf( "%s", tmp);

    MONOP_EXPR( arg_node) = TRAVdo( MONOP_EXPR( arg_node), arg_info);

    printf( ")");

    DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTfloat
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Float node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfloat (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfloat");

  printf( "%f", FLOAT_VALUE( arg_node));

  DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn PRTint
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Num node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTint (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTint");

  printf( "%i", INT_VALUE( arg_node));

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTboolean
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Boolean node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbool (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTbool");

  if (BOOL_VALUE( arg_node)) {
    printf( "true");
  }
  else {
    printf( "false");
  }

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvar
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvar (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvar");

  node *dimensions;

  printf( "%s", VAR_NAME( arg_node));

  dimensions = VAR_DIMENSIONS(arg_node);

  if (dimensions != NULL) {
    printf("[");

    dimensions = TRAVdo(dimensions, arg_info);

    printf("]");
  }

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTsymboltableentry
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *PRTsymboltableentry (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTsymboltableentry");

  DBUG_RETURN (arg_node);
}

/** <!--******************************************************************-->
 *
 * @fn PRTerror
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node * PRTerror (node * arg_node, info * arg_info)
{
  bool first_error;

  DBUG_ENTER ("PRTerror");

  if (NODE_ERROR (arg_node) != NULL) {
    NODE_ERROR (arg_node) = TRAVdo (NODE_ERROR (arg_node), arg_info);
  }

  first_error = INFO_FIRSTERROR( arg_info);

  if( (global.outfile != NULL)
      && (ERROR_ANYPHASE( arg_node) == global.compiler_anyphase)) {

    if ( first_error) {
      printf ( "\n/******* BEGIN TREE CORRUPTION ********\n");
      INFO_FIRSTERROR( arg_info) = FALSE;
    }

    printf ( "%s\n", ERROR_MESSAGE( arg_node));

    if (ERROR_NEXT (arg_node) != NULL) {
      TRAVopt (ERROR_NEXT (arg_node), arg_info);
    }

    if ( first_error) {
      printf ( "********  END TREE CORRUPTION  *******/\n");
      INFO_FIRSTERROR( arg_info) = TRUE;
    }
  }

  DBUG_RETURN (arg_node);
}


static void print_type(type ty) {
    char *tmp;

    switch (ty) {
    case TY_bool:
        tmp = "bool";
        break;
    case TY_float:
        tmp = "float";
        break;
    case TY_int:
        tmp = "int";
        break;
    case TY_void:
        tmp = "void";
        break;
    case TY_unknown:
        DBUG_ASSERT( 0, "unknown type detected!");
    }
    printf("%s", tmp);
}

static void print_global_prefix(enum global_prefix pfx) {
    char *tmp;

    switch (pfx) {
    case global_prefix_none:
        return;
    case global_prefix_export:
        tmp = "export";
        break;
    case global_prefix_extern:
        tmp = "extern";
        break;
    case global_prefix_var:
        return;
    }
    printf("%s ", tmp);
}

node *PRTprogram (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTprogram");

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);

    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTinnerblock (node * arg_node, info * arg_info)
{
    node *var;
    node *stmts;

    DBUG_ENTER("PRTinnerblock");

    var = INNERBLOCK_VARS(arg_node);

    if (var != NULL) {
        var = TRAVdo(var, arg_info);
    }

    stmts = INNERBLOCK_STMTS(arg_node);

    if (stmts != NULL) {
        stmts = TRAVdo(stmts, arg_info);
    }

    //print_blocklike(&BLOCK_STMTS(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTvardeflist (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTvardeflist");

    VARDEFLIST_HEAD(arg_node) = TRAVdo(VARDEFLIST_HEAD(arg_node), arg_info);

    VARDEFLIST_NEXT(arg_node) = TRAVopt(VARDEFLIST_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTvardef (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTvardef");

    indent(arg_info);

    print_global_prefix(VARDEF_PREFIX(arg_node));
    print_type(VARDEF_TY(arg_node));

    if (VARDEF_DIMENSIONCOUNT(arg_node) != 0) {
      printf("[%d]", VARDEF_DIMENSIONCOUNT(arg_node));
    }

    printf(" %s", VARDEF_ID(arg_node));
    node *init = VARDEF_INIT(arg_node);
    if (init != NULL) {
        printf(" = ");
        VARDEF_INIT(arg_node) = TRAVdo(init, arg_info);
    }

    printf(";\n");
    newline(arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTfun (node * arg_node, info * arg_info)
{
    node *body;

    DBUG_ENTER("PRTfun");

    indent(arg_info);

    print_global_prefix(FUN_PREFIX(arg_node));
    print_type(FUN_RETTY(arg_node));
    printf(" %s(", FUN_ID(arg_node));

    FUN_PARAMS(arg_node) = TRAVopt(FUN_PARAMS(arg_node), arg_info);

    printf(")");

    body = FUN_BODY(arg_node);

    if (body != NULL) {
        printf(" {\n");
        INDENT_INCREASE(arg_info);
        body = TRAVdo(body, arg_info);
        INDENT_DECREASE(arg_info);
        printf("}\n");
    } else {
        printf(";\n");
    }

    DBUG_RETURN(arg_node);
}

node *PRTfunparamlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTfunparamlist");

    FUNPARAMLIST_PARAM(arg_node) = TRAVdo(FUNPARAMLIST_PARAM(arg_node), arg_info);

    if (FUNPARAMLIST_NEXT(arg_node) != NULL) {
        printf(", ");
        FUNPARAMLIST_NEXT(arg_node) = TRAVdo(FUNPARAMLIST_NEXT(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *PRTfunparam (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTfunparam");

    node *dimensions;

    print_type(FUNPARAM_TY(arg_node));

    dimensions = FUNPARAM_DIMENSIONS(arg_node);

    if (dimensions != NULL) {
        dimensions = TRAVdo(dimensions, arg_info);
    }

    printf(" %s", FUNPARAM_ID(arg_node));

    DBUG_RETURN(arg_node);
}

node *PRTif (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTif");

    indent(arg_info);

    printf("if (");
    IF_COND(arg_node) = TRAVdo(IF_COND(arg_node), arg_info);
    printf(") ");
    print_blocklike(&IF_BLOCKT(arg_node), arg_info);
    if (IF_BLOCKF(arg_node) != 0) {
        printf(" else ");
        print_blocklike(&IF_BLOCKF(arg_node), arg_info);
    }
    printf("\n"); newline(arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTwhile (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTwhile");

    indent(arg_info);
    printf("while (");
    WHILE_COND(arg_node) = TRAVdo(WHILE_COND(arg_node), arg_info);
    printf(") ");
    print_blocklike(&WHILE_BLOCK(arg_node), arg_info);
    printf("\n"); newline(arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTdowhile (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTdowhile");

    indent(arg_info);
    printf("do ");
    print_blocklike(&DOWHILE_BLOCK(arg_node), arg_info);
    indent(arg_info);
    printf(" while (");
    DOWHILE_COND(arg_node) = TRAVdo(DOWHILE_COND(arg_node), arg_info);
    printf(");\n"); newline(arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTfor (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTfor");

    indent(arg_info);
    printf("for (");
    INFO_NO_SEMICOLON(arg_info) = TRUE;
    FOR_ASSIGN(arg_node) = TRAVdo(FOR_ASSIGN(arg_node), arg_info);
    INFO_NO_SEMICOLON(arg_info) = FALSE;
    printf(", ");
    FOR_UPPER(arg_node) = TRAVdo(FOR_UPPER(arg_node), arg_info);

    if (FOR_STEP(arg_node) != NULL) {
        printf(", ");
        FOR_STEP(arg_node) = TRAVdo(FOR_STEP(arg_node), arg_info);
    }

    printf(") ");
    print_blocklike(&FOR_BLOCK(arg_node), arg_info);
    printf("\n");
    newline(arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTreturn (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTreturn");

    indent(arg_info);
    printf("return ");
    RETURN_EXPR(arg_node) = TRAVdo(RETURN_EXPR(arg_node), arg_info);
    printf(";\n"); newline(arg_info);

    DBUG_RETURN(arg_node);
}

node *PRTexprlist (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTexprlist");

    EXPRLIST_EXPR( arg_node) = TRAVdo( EXPRLIST_EXPR( arg_node), arg_info);
    if (EXPRLIST_NEXT( arg_node) != NULL) {
        printf(", ");
        EXPRLIST_NEXT( arg_node) = TRAVdo( EXPRLIST_NEXT( arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *PRTcall (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTcall");

    printf("%s(", VAR_NAME(CALL_ID(arg_node)));
    CALL_ARGS( arg_node) = TRAVopt( CALL_ARGS( arg_node), arg_info);
    printf(")");

    DBUG_RETURN(arg_node);
}

node *PRTcast (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTcast");

    printf("( (");
    print_type(CAST_TY(arg_node));
    printf(")");
    CAST_EXPR( arg_node) = TRAVdo( CAST_EXPR( arg_node), arg_info);
    printf(")");

    DBUG_RETURN(arg_node);
}

node *PRTarray (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTarray");

    printf("[");

    TRAVopt(ARRAY_ITEMS(arg_node), arg_info);

    printf("]");

    DBUG_RETURN(arg_node);
}

node *PRTnewarray (node * arg_node, info * arg_info)
{
    DBUG_ENTER("PRTnewarray");

    DBUG_RETURN(arg_node);
}




/** <!-- ****************************************************************** -->
 * @brief Prints the given syntaxtree
 *
 * @param syntaxtree a node structure
 *
 * @return the unchanged nodestructure
 ******************************************************************************/

node *PRTdoPrint( node *syntaxtree)
{
  info *info;

  DBUG_ENTER("PRTdoPrint");

  DBUG_ASSERT( (syntaxtree!= NULL), "PRTdoPrint called with empty syntaxtree");

  printf( "\n\n------------------------------\n\n");

  info = MakeInfo();

  TRAVpush( TR_prt);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  info = FreeInfo(info);

  printf( "\n------------------------------\n\n");

  DBUG_RETURN( syntaxtree);
}

/**
 * @}
 */
