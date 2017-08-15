%{


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

extern int yylex();
static int yyerror( char *errname);

static node *parseresult = NULL;
static node *global_scope = NULL;
static node *cur_scope = NULL;

static bool cur_scope_is_global() {
    return BLOCK_PARENT(cur_scope) == NULL;
}

%}

%union {
    node               *node;
    nodetype            nodetype;
    char               *id;
    int                 cint;
    float               cflt;
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

%token <cint> INTVAL
%token <cflt> FLOATVAL
%token <id> ID

%type <node> program vardef fundef funheader fun_params fun_param fun_body_defs fun_body_vardefs
%type <node> stmts stmt var assign stmt_return
%type <node> stmt_if stmt_if_else stmt_while stmt_do_while stmt_for
%type <node> call_args call
%type <node> vardeflist
%type <node> expr expr0 expr2 expr3 expr4 expr6 expr7 expr11 expr12
%type <cbinop> binop3 binop4 binop6 binop7 binop11 binop12

%type <cmonop> monop2
%type <ctype> ty
%type <cglobal_prefix> global_prefix

%start program

%%

ty: BOOL    { $$ = TY_bool; }
  | INT     { $$ = TY_int; }
  | FLOAT   { $$ = TY_float; }
  | VOID    { $$ = TY_void; }
  ;


program: {
            DBUG_ASSERT(cur_scope == NULL, "program is not reentrant");
            global_scope = TBmakeBlock(NULL, NULL);
            cur_scope = global_scope;
            BLOCK_FUNSTAIL(global_scope) = &BLOCK_FUNS(global_scope);
         }
         program_
         {
            DBUG_ASSERT(cur_scope_is_global(), "current scope is not global");
            parseresult = TBmakeProgram(global_scope);
            cur_scope = NULL;
         }
       ;
program_: global_prefix fundef {    FUN_PREFIX($2) = $1; } program_ {}
        | global_prefix vardef { VARDEF_PREFIX($2) = $1; } program_ {}
        |                                                           {}
        ;

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

vardef: ty ID LET expr SEMICOLON
        {
            $$ = TBmakeVardef($1, $2, $4);
        }
      | ty ID SEMICOLON
        {
            $$ = TBmakeVardef($1, $2, NULL);
        };


fun_params: fun_param COMMA fun_params  { 
                $$ = TBmakeFunparamlist($1, $3); 
            }
            | fun_param
            { 
                $$ = TBmakeFunparamlist($1, NULL);
            };

fun_param: ty ID
    {
        $$ = TBmakeFunparam($1, $2);
    };

funheader: ty ID BRACKET_L fun_params BRACKET_R
    {  
         node *scope = TBmakeInnerblock(NULL, NULL);
         INNERBLOCK_PARENT(scope) = cur_scope;
         BLOCK_FUNSTAIL(global_scope) = &BLOCK_FUNS(global_scope);
         cur_scope = scope;

         $$ = TBmakeFun($1, $2, $4, NULL, NULL);
    };

fundef: funheader ANBRACKET_L fun_body_defs stmts ANBRACKET_R
    {
         node *body = TBmakeInnerblock($3, $4);
         cur_scope = INNERBLOCK_PARENT(cur_scope);

         node *x = $1;
         FUN_BODY(x) = body;
         *BLOCK_FUNSTAIL(global_scope) = x;
         BLOCK_FUNSTAIL(global_scope) = &FUN_NEXT(x);
         $$ = x;
    };

fun_body_defs: fun_body_vardefs           { $$ = $1; };
fun_body_vardefs: vardeflist fun_body_vardefs { $$ = $1; }
                |                         { $$ = NULL; }

stmts: stmt stmts       { $$ = TBmakeStmts($1, $2); }
     |                  { $$ = NULL; }
     ;
stmt: assign            { $$ = $1; }
    | call SEMICOLON    { $$ = TBmakeAssign(NULL, $1); }
    | stmt_if           { $$ = $1; }
    | stmt_while        { $$ = $1; }
    | stmt_do_while     { $$ = $1; }
    | stmt_for          { $$ = $1; }
    | stmt_return       { $$ = $1; }
    ;

var: ID { $$ = TBmakeVar($1); };

assign: var LET expr SEMICOLON  { $$ = TBmakeAssign($1, $3); }
      | var LET expr            { $$ = TBmakeAssign($1, $3); }

call: var BRACKET_L call_args BRACKET_R  { $$ = TBmakeCall($1, $3); };
call_args: expr COMMA call_args         { $$ = TBmakeExprlist($1, $3); }
         | expr                         { $$ = TBmakeExprlist($1, NULL); }
         |                              { $$ = NULL; }
         ;

stmt_return: RETURN expr SEMICOLON      { $$ = TBmakeReturn($2); };

stmt_if: IF BRACKET_L expr BRACKET_R ANBRACKET_L stmts ANBRACKET_R stmt_if_else
            { $$ = TBmakeIf($3, $6, $8); }
            ;
stmt_if_else: ELSE ANBRACKET_L stmts ANBRACKET_R    { $$ = $3; }
            |                                       { $$ = NULL; }
            ;

stmt_while: WHILE BRACKET_L expr BRACKET_R ANBRACKET_L stmts ANBRACKET_R
                { $$ = TBmakeWhile($3, $6); };

stmt_do_while: DO ANBRACKET_L stmts ANBRACKET_R WHILE BRACKET_L expr BRACKET_R SEMICOLON
                { $$ = TBmakeDowhile($3, $7); };

stmt_for: FOR BRACKET_L INT var LET expr COMMA expr COMMA expr BRACKET_R ANBRACKET_L stmts ANBRACKET_R
                { 
                    $$ = TBmakeFor(TBmakeAssign($4, $6), $8, $10, $13); 
                }
                | FOR BRACKET_L INT var LET expr COMMA expr BRACKET_R ANBRACKET_L stmts ANBRACKET_R
                {
                    $$ = TBmakeFor(TBmakeAssign($4, $6), $8, NULL, $11); 
                };

expr: expr12 { $$ = $1; };
expr12: expr12 binop12 expr11           { $$ = TBmakeBinop($2, $1, $3); }
      | expr11                          { $$ = $1; }
      ;
expr11: expr11 binop11 expr7            { $$ = TBmakeBinop($2, $1, $3); }
     | expr7                            { $$ = $1; }
     ;
expr7: expr7 binop7 expr6               { $$ = TBmakeBinop($2, $1, $3); }
     | expr6                            { $$ = $1; }
     ;
expr6: expr6 binop6 expr4               { $$ = TBmakeBinop($2, $1, $3); }
     | expr4                            { $$ = $1; }
     ;
expr4: expr4 binop4 expr3               { $$ = TBmakeBinop($2, $1, $3); }
     | expr3                            { $$ = $1; }
     ;
expr3: expr3 binop3 expr2               { $$ = TBmakeBinop($2, $1, $3); }
     | expr2                            { $$ = $1; }
     ;
expr2: BRACKET_L ty BRACKET_R expr2     { $$ = TBmakeCast($2, $4); }
     | monop2 expr2                     { $$ = TBmakeMonop($1, $2); }
     | expr0                            { $$ = $1; }
     ;
expr0: call                             { $$ = $1; }
     | var                              { $$ = $1; }
     | FLOATVAL                         { $$ = TBmakeFloat($1); }
     | INTVAL                           { $$ = TBmakeInt($1); }
     | TRUEVAL                          { $$ = TBmakeBool(TRUE); }
     | FALSEVAL                         { $$ = TBmakeBool(FALSE); }
     | BRACKET_L expr BRACKET_R         { $$ = $2; }
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

    yyparse();

    DBUG_RETURN( parseresult);
}
