%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "node_basic.h"
#include "str.h"
#include "free.h"
#include "civic.tab.h"
#include "globals.h"
#include "ctinfo.h"


#define FILTER(token) \
  global.col += yyleng;  \
  return( token);

#define LINEBUF_SIZE 256
#define YY_NO_UNPUT
#define YY_NO_INPUT


%}

%p 6000 /* set positions to 6000 */
%e 2000 /* set nodes to 1100 */
%option noyywrap
%option nounput


%%

"("                        { FILTER( BRACKET_L); }
")"                        { FILTER( BRACKET_R); }
"{"                        { FILTER( ANBRACKET_L); }
"}"                        { FILTER( ANBRACKET_R); }
"["                        { FILTER( SQBRACKET_L); }
"]"                        { FILTER( SQBRACKET_R); }
","                        { FILTER( COMMA); }
";"                        { FILTER( SEMICOLON); }

"-"                        { FILTER( MINUS); }
"+"                        { FILTER( PLUS);  }
"*"                        { FILTER( STAR);  }
"/"                        { FILTER( SLASH);  }
"%"                        { FILTER( PERCENT);  }

"<="                       { FILTER( LE);    }
"<"                        { FILTER( LT);    }
">="                       { FILTER( GE);    }
">"                        { FILTER( GT);    }
"=="                       { FILTER( EQ);    }
"!="                       { FILTER( NE);    }
"&&"                       { FILTER( AND);    }
"||"                       { FILTER( OR);    }

"="                        { FILTER( LET);    }
"!"                        { FILTER( NOT);  }

"bool"                     { FILTER( BOOL); }
"do"					   { FILTER( DO); }
"else"					   { FILTER( ELSE); }
"extern"                   { FILTER( EXTERN); }
"export"                   { FILTER( EXPORT); }
"false"                    { FILTER( FALSEVAL); }
"float"                    { FILTER( FLOAT); }
"for"					   { FILTER( FOR); }
"if"					   { FILTER( IF); }
"int"                      { FILTER( INT); }
"return"				   { FILTER( RETURN); }
"true"                     { FILTER( TRUEVAL); }
"void"                     { FILTER( VOID); }
"while"                    { FILTER( WHILE); }


[A-Za-z][A-Za-z0-9_]*      { yylval.id = STRcpy(yytext);
                             FILTER( ID);
                           }

[0-9]+                     { yylval.cint=atoi(yytext);
                             FILTER( INTVAL);
                           }

[0-9]+\.[0-9]+             { yylval.cflt=atof(yytext);
                             FILTER( FLOATVAL);
                           }

\n.*                       { global.line += 1;
                             global.col = 0;
                             yyless(1);
                           }

[ \t]                      { global.col += yyleng;
                           }
