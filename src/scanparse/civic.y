%{


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "str.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"
#include "globals.h"

static node *parseresult = NULL;

extern int yylex();
static int yyerror( char *errname);

%}

%union {
    node               *node; 
    nodetype            nodetype;
    char               *id;
    int                 cint;
    float               cflt;
    binop               cbinop;
    monop               cminop;
    type                ctype;
    enum global_prefix  cglobal_prefix;
}

%token BRACKET_L BRACKET_R COMMA SEMICOLON ANBRACKET_L ANBRACKET_R SQBRACKET_L SQBRACKET_R 
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND
%token TRUEVAL FALSEVAL LET
%token WHILE DO FOR IF ELSE RETURN BOOL
%token NOT EXTERN EXPORT VOID FLOAT INT

%token <cint> INTVAL
%token <cflt> FLOATVAL
%token <id> ID

%type <node> program vardef fun fun_params fun_body_defs fun_body_vardefs
%type <node> stmts stmt var assign stmt_return
%type <node> stmt_if stmt_if_else stmt_while stmt_do stmt_for stmt_for_opt
%type <node> expr
%type <cbinop> binop
%type <cmonop> monop
%type <ctype> ty
%type <cglobal_prefix> global_prefix
%type <node> vardef_init


%start program

%%

ty: BOOL    { $$ = TY_bool; }
  | INT     { $$ = TY_INT; }
  | FLOAT   { $$ = TY_float; } 
  | VOID    { $$ = TY_void; };

program: stmts 
         {
           parseresult = $1;
         }
         ;

global_prefix: EXTERN { $$ = global_prefix_extern; }
             | EXPORT { $$ = global_prefix_export; }
             |        { $$ = global_prefix_none; };

vardef: ty ID vardef_init SEMICOLON
        {
            $$ = $3;
        };

vardef_init: LET expr { $$ = $2; }
           |          { $$ = NULL; };
            

stmts: stmt stmts
        {
          $$ = TBmakeStmts( $1, $2);
        }
      | { $$ = NULL ;};

stmt:   assign
        {
            $$ = $1;
        }
    |   stmt_if
        {
            $$ = $1;
        }
    |   stmt_while
        {
            $$ = $1;
        }
    |   stmt_do
        {
            $$ = $1;
        }
    |   stmt_for
        {
            $$ = $1;
        }
    |   stmt_return
        {
            $$ = $1;
        };

var: ID { $$ = TBmakeVar($1); };

assign: var LET expr SEMICOLON
        {
          $$ = TBmakeAssign( $1, $3);
        }
        ;

stmt_return: RETURN expr SEMICOLON 
             {
                $$ = TBmakeReturn($2);
             };

stmt_if: IF BRACKET_L expr BRACKET_R ANBRACKET_L stmts ANBRACKET_R stmt_if_else
        { $$ = TBmakeIf($3, $6, $8); };

stmt_if_else: ELSE ANBRACKET_L stmts ANBRACKET_R    { $$ = $3; }
            |                                       { $$ = NULL; };

stmt_while: WHILE BRACKET_L expr BRACKET_R ANBRACKET_L stmts ANBRACKET_R
        { $$ = TBmakeWhile($3, $6); };

stmt_do: DO ANBRACKET_L stmts ANBRACKET_R WHILE BRACKET_L expr BRACKET_R SEMICOLON
        { $$ = TBmakeDo($3, $7); };

stmt_for: FOR BRACKET_L INT ID LET expr COMMA expr stmt_for_opt BRACKET_R ANBRACKET_L stmts ANBRACKET_R
        { $$ = TBmakeFor ($4, $6, $8, $9); };

stmt_for_opt: COMMA expr    { $$ = $2; }
            |               { $$ = NULL; };

expr: ID
      {
        $$ = TBmakeVar( STRcpy( $1));
      }
    | BRACKET_L expr binop expr BRACKET_R
      {
        $$ = TBmakeBinop( $3, $2, $4);
      }
    | INTVAL<F3>
      {
        $$ = TBmakeNum($1);
      }
    | FLOATVAL
      {
        $$ = TBmakeFloat($1);
      }     
    ;

fun_params: ty ID COMMA fun_params { $$ = TBmakeFunParam($1, $2, $4); }
          | ty ID                  { $$ = TBmakeFunParam($1, $2, NULL); }
          |                        { $$ = NULL; };

fun: ty ID BRACKET_L fun_params BRACKET_R
    {
    }
    ANBRACKET_L fun_body_defs stmts ANBRACKET_R
    {
    };

fun_body_defs: fun_body_vardefs     { $$ = $1; };

fun_body_vardefs: vardef fun_body_vardefs { $$ = $1; }

binop: PLUS      { $$ = BO_add; }
     | MINUS     { $$ = BO_sub; }
     | STAR      { $$ = BO_mul; }
     | SLASH     { $$ = BO_div; }
     | PERCENT   { $$ = BO_mod; }
     | LE        { $$ = BO_le; }
     | LT        { $$ = BO_lt; }
     | GE        { $$ = BO_ge; }
     | GT        { $$ = BO_gt; }
     | EQ        { $$ = BO_eq; }
     | NE        { $$ = BO_ne; }
     | OR        { $$ = BO_or; }
     | AND       { $$ = BO_and; }
     ;

monop:      MINUS { $$ = MO_neg; }
     |      NOT   { $$ = MO_NOT; };
      
%%

static int yyerror( char *error)
{
  CTIabort( "line %d, col %d\nError parsing source code: %s\n", 
            global.line, global.col, error);

  return( 0);
}

node *YYparseTree( void)
{
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}

