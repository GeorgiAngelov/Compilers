%{
#include "tokens.h"
#include <string.h>
%}

/*digit is just a shorthand/definition to be used when defining regular expressions*/
digit [0-9]

/*noyywrap option invoked*/
%option noyywrap


/*Regular expressions for generating tokens*/

%%

";"                            {return SEMICOLON;}
":"                            {return COLON;}
"="                            {return EQ;}
"("                            {return LPAREN;}
")"                            {return RPAREN;}
"+"                            {return PLUS;}
"-"                            {return MINUS;}
"begin"                        {return MYBEGIN;}
"end"                          {return END;}
"print"                        {return PRINT;}
{digit}+                       {return NUM;}
[A-Za-z][A-Za-z0-9"_"]*        {return ID;}
" "                            {}
"\n"                           {}
.                              {printf( "illegal token(%s)\n", yytext);}

%%

