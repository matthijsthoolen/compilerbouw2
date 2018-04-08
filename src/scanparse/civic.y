%{
#define YYDEBUG 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "node_basic.h"
#include "str.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"
#include "globals.h"
#include "numbers.h"

extern int yylex();
static int yyerror( char *errname);

static node *parseresult = NULL;

%}

%union {
    node               *node;
    nodetype            nodetype;
    char               *id;
    char               *cint;
    char               *cflt;
    binop               cbinop;
    monop               cmonop;
    type                ctype;
    enum global_prefix  cglobal_prefix;
}

%token BRACKET_L BRACKET_R COMMA SEMICOLON
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND
%token TRUEVAL FALSEVAL LET
%token WHILE DO FOR IF ELSE RETURN BOOL
%token ANBRACKET_L ANBRACKET_R SQBRACKET_L SQBRACKET_R
%token NOT EXTERN EXPORT VOID FLOAT INT

%token <cint> INTVALUE
%token <cflt> FLOATVALUE
%token <id> ID

%type <node> decl
%type <node> program
%type <node> vardef fundef funheader fun_params fun_param
%type <node> stmts vardeflist funbody block
%type <node> stmt var assign stmt_return
%type <node> stmt_if stmt_if_else stmt_while stmt_do_while stmt_for
%type <node> call_args call
%type <node> numberint numberfloat boolval constant
%type <node> expr expr0 expr2 expr3 expr4 expr6 expr7 expr11 expr12 expr_array array_vars array_var
%type <cbinop> binop3 binop4 binop6 binop7 binop11 binop12

%type <cmonop> monop2
%type <ctype> ty
%type <cglobal_prefix> global_prefix

%start result

%%

ty: BOOL    { $$ = TY_bool; }
  | INT     { $$ = TY_int; }
  | FLOAT   { $$ = TY_float; }
  | VOID    { $$ = TY_void; }
  ;

result: program
    {
        parseresult = $1;
    };

program: decl program
    {
        $$ = TBmakeProgram($1, $2, TBmakeSymboltable(NULL));
    }
    | decl
    {
        $$ = TBmakeProgram($1, NULL, TBmakeSymboltable(NULL));
    };

decl: fundef
    {
        $$ = $1;
    }
    | vardef
    {
        VARDEF_PREFIX($1) = global_prefix_var;
        $$ = $1;
    };

global_prefix: EXTERN   { $$ = global_prefix_extern; }
             | EXPORT   { $$ = global_prefix_export; }
             |          { $$ = global_prefix_none; }
             ;

vardeflist: vardef vardeflist
     {
         $$ = TBmakeVardeflist($1, $2);
     }
     | vardef
     {
         $$ = TBmakeVardeflist($1, NULL);
     };

vardef: global_prefix ty ID LET expr SEMICOLON
    {
        $$ = TBmakeVardef($1, $2, $3, NULL, $5, TBmakeInt(0));
    }
    | global_prefix ty ID LET expr_array SEMICOLON
    {
        $$ = TBmakeVardef($1, $2, $3, NULL, $5, TBmakeInt(0));
    }
    | global_prefix ty SQBRACKET_L numberint SQBRACKET_R ID LET expr SEMICOLON
    {
        $$ = TBmakeVardef($1, $2, $6, NULL, $8, $4);
    }
    | global_prefix ty SQBRACKET_L numberint SQBRACKET_R ID SEMICOLON
    {
        $$ = TBmakeVardef($1, $2, $6, NULL, NULL, $4);
    }
    | global_prefix ty SQBRACKET_L SQBRACKET_R ID SEMICOLON
    {
        $$ = TBmakeVardef($1, $2, $5, NULL, NULL, TBmakeInt(0));
    }
    | global_prefix ty ID SEMICOLON
    {
        $$ = TBmakeVardef($1, $2, $3, NULL, NULL, TBmakeInt(0));
    };

funheader: global_prefix ty ID BRACKET_L fun_params BRACKET_R
    {
         $$ = TBmakeFun($1, $2, $3, $5, NULL, NULL);
    };

fun_params: fun_param COMMA fun_params
    {
        $$ = TBmakeFunparamlist($1, $3);
    }
    | fun_param
    {
        $$ = TBmakeFunparamlist($1, NULL);
    }
    |
    {
        $$ = NULL;
    };

fun_param: ty expr_array ID
    {
        $$ = TBmakeFunparam($1, $3, $2);
    }
    | ty ID {
        $$ = TBmakeFunparam($1, $2, NULL);
    };

block: ANBRACKET_L stmts ANBRACKET_R { $$ = $2; }
     | ANBRACKET_L ANBRACKET_R       { $$ = NULL; }
     | stmt                          { $$ = TBmakeStmts($1, NULL); }
     ;

funbody: ANBRACKET_L vardeflist stmts ANBRACKET_R
    {
         $$ = TBmakeInnerblock($2, $3);
    }
    | ANBRACKET_L vardeflist ANBRACKET_R
    {
         $$ = TBmakeInnerblock($2, NULL);
    }
    | block
    {
         $$ = TBmakeInnerblock(NULL, $1);
    }
    ;

fundef: funheader funbody
    {
         node *x = $1;
         node *body = $2;
         FUN_BODY(x) = body;
         $$ = x;
    }
    | funheader SEMICOLON
    {
        $$ = $1;
    }
    ;

stmts: stmt stmts       { $$ = TBmakeStmts($1, $2); }
     |                  { $$ = NULL; }
     ;

stmt: assign            { $$ = $1; }
    | call SEMICOLON    { $$ = $1; }
    | stmt_if           { $$ = $1; }
    | stmt_while        { $$ = $1; }
    | stmt_do_while     { $$ = $1; }
    | stmt_for          { $$ = $1; }
    | stmt_return       { $$ = $1; }
    ;

var: ID expr_array { $$ = TBmakeVar($1, $2); }
   | ID            { $$ = TBmakeVar($1, NULL); }
   ;

assign: var LET expr_array SEMICOLON { $$ = TBmakeAssign($1, $3); }
      | var LET expr SEMICOLON       { $$ = TBmakeAssign($1, $3); }
      | var LET expr                 { $$ = TBmakeAssign($1, $3); }
      ;

call: ID BRACKET_L call_args BRACKET_R  { $$ = TBmakeCall($1, $3); };
call_args: expr COMMA call_args          { $$ = TBmakeExprlist($1, $3); }
         | expr                          { $$ = TBmakeExprlist($1, NULL); }
         |                               { $$ = NULL; }
         ;

stmt_return: RETURN expr SEMICOLON      { $$ = TBmakeReturn($2); }
           | RETURN SEMICOLON           { $$ = TBmakeReturn(NULL); }
           ;

stmt_if: IF BRACKET_L expr BRACKET_R block stmt_if_else
       {
            $$ = TBmakeIf($3, $5, $6);
       }
       ;

stmt_if_else: ELSE block   { $$ = $2; }
            |              { $$ = NULL; }
            ;

stmt_while: WHILE BRACKET_L expr BRACKET_R block
                { $$ = TBmakeWhile($3, $5); };

stmt_do_while: DO block WHILE BRACKET_L expr BRACKET_R SEMICOLON
                { $$ = TBmakeDowhile($2, $5); };

stmt_for: FOR BRACKET_L INT var LET expr COMMA expr COMMA expr BRACKET_R block
                {
                    $$ = TBmakeFor(TBmakeAssign($4, $6), $8, $10, $12);
                }
                | FOR BRACKET_L INT var LET expr COMMA expr BRACKET_R block
                {
                    $$ = TBmakeFor(TBmakeAssign($4, $6), $8, TBmakeInt(1), $10);
                };

expr: expr12 { $$ = $1; };
expr12: expr12 binop12 expr11           { $$ = TBmakeBinop(TY_unknown, $2, $1, $3); }
      | expr11                          { $$ = $1; }
      ;
expr11: expr11 binop11 expr7            { $$ = TBmakeBinop(TY_unknown, $2, $1, $3); }
     | expr7                            { $$ = $1; }
     ;
expr7: expr7 binop7 expr6               { $$ = TBmakeBinop(TY_unknown, $2, $1, $3); }
     | expr6                            { $$ = $1; }
     ;
expr6: expr6 binop6 expr4               { $$ = TBmakeBinop(TY_unknown, $2, $1, $3); }
     | expr4                            { $$ = $1; }
     ;
expr4: expr4 binop4 expr3               { $$ = TBmakeBinop(TY_unknown, $2, $1, $3); }
     | expr3                            { $$ = $1; }
     ;
expr3: expr3 binop3 expr2               { $$ = TBmakeBinop(TY_unknown, $2, $1, $3); }
     | expr2                            { $$ = $1; }
     ;
expr2: BRACKET_L ty BRACKET_R expr2     { $$ = TBmakeCast($2, $4); }
     | monop2 expr2                     { $$ = TBmakeMonop(TY_unknown, $1, $2); }
     | expr0                            { $$ = $1; }
     ;
expr0: call                             { $$ = $1; }
     | var                              { $$ = $1; }
     | constant                         { $$ = $1; }
     | BRACKET_L expr BRACKET_R         { $$ = $2; }
     ;

constant: numberint   { $$ = $1; }
        | numberfloat { $$ = $1; }
        | boolval     { $$ = $1; }
        ;

boolval:  TRUEVAL  { $$ = TBmakeBool(TRUE); }
       |  FALSEVAL { $$ = TBmakeBool(FALSE); }
       ;

numberint: INTVALUE {
    int* value = convertToInt($1);
    MEMfree($1);

    if (value == NULL) {
        yyerror("Integer value out of range (signed 32-bit)");
    }

    $$ = TBmakeInt(*value);
    free(value);
}

numberfloat: FLOATVALUE {
    float* value = convertToFloat($1);
    MEMfree($1);

    if (value == NULL) {
        yyerror("Float value out of range (signed 32-bit)");
    }

    $$ = TBmakeFloat(*value);
    free(value);
}

 expr_array: SQBRACKET_L array_vars SQBRACKET_R { $$ = TBmakeArray($2); };
 array_vars: array_var COMMA array_vars         { $$ = TBmakeExprlist($1, $3); }
           | array_var                          { $$ = TBmakeExprlist($1, NULL); }
           ;
 array_var: expr_array                          { $$ = $1; }
          | expr                                { $$ = $1; }
          ;

binop12:         OR { $$ = BO_or; };
binop11:        AND { $$ = BO_and; };
binop7:          EQ { $$ = BO_eq; }
      |          NE { $$ = BO_ne; }
      ;
binop6:          LT { $$ = BO_lt; }
      |          LE { $$ = BO_le; }
      |          GT { $$ = BO_gt; }
      |          GE { $$ = BO_ge; }
      ;
binop4:        PLUS { $$ = BO_add; }
      |       MINUS { $$ = BO_sub; }
      ;
binop3:        STAR { $$ = BO_mul; }
      |       SLASH { $$ = BO_div; }
      |     PERCENT { $$ = BO_mod; }
      ;
monop2:       MINUS { $$ = MO_neg; }
      |         NOT { $$ = MO_not; }
      ;


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

    // Correct for global.line starting on line 0
    global.line = 1;
    yydebug = 0;

    yyparse();

    DBUG_RETURN( parseresult);
}
