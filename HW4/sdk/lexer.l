%{

#include "parser.h"
#include "ast.h"
#include <math.h>
#include <ctype.h>

#define MAX_STR 255
#define MAX_ID 32

%}

%option noyywrap
%option nounput
%option noinput

DIGIT             [0-9]
ID                [a-zA-Z][_a-zA-Z0-9]*

SHORT_COMP_OP     "<"|">"
ARITH_OP          "+"|"-"|"/"|"%"|"*"
BOOL_OP           "|"|"&"|"!"
ASSIGN            "="
PUNCTUATION       "("|")"|"["|"]"|"{"|"}"|":"|";"|","|"."
SHORT_TOK         {SHORT_COMP_OP}|{ARITH_OP}|{BOOL_OP}|{ASSIGN}|{PUNCTUATION}

%%

"/*"([^*]|("*"+([^*/])))*"*"+"/" { /* Eat multi-line comments. */ }

"//"[^\r\n]* { /* Eat single-line comments. */ }

"var"       { return T_VAR; }
"type"      { return T_TYPE; }
"function"  { return T_FUNCTION; }
"int"       { return T_INT; }
"bool"      { return T_BOOL; }

"for"       { return T_FOR; }
"to"        { return T_TO; }
"while"     { return T_WHILE; }
"if"        { return T_IF; }
"else"      { return T_ELSE; }
"return"    { return T_RETURN; }

"nil"       { return T_NIL; }
"true"      { return T_TRUE; }
"false"     { return T_FALSE; }

"<="        { return T_LT_EQ; }
">="        { return T_GT_EQ; }
"=="        { return T_EQ; }
"!="        { return T_NOT_EQ; }

{SHORT_TOK} { return *yytext; }

{DIGIT}+ {
      yylval.num = atoi(yytext);
      return T_NUM;
}

\"(\\\"|\\\\|[^"\r\n\\])*\" {
      // Chop off the quotes.
      yylval.str = strndup(yytext + 1, MAX_STR);
      yylval.str[strlen(yylval.str)-1] = 0;
      return T_STR;
}

{ID} {
      yylval.id = strndup(yytext, MAX_ID);
      return T_ID;
}

[ \n\r\t]+ {
      // Eat up whitespace.
}

. {
      fprintf(stderr, "Error: unrecognized character: %s\n", yytext);
      return T_UNKNOWN;
}

%%

