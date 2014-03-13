%{
#include <string.h>
#include "microextendedcount_and_idlist.tab.h"

%}

%option noyywrap

%%
[" "\t\n]	 {continue;}
";"	         {return SEMICOLON;}
":"	         {return COLON;}
"="	         {return EQ;}
"+"	         {return PLUS;}
"-"	         {return MINUS;}
"*"	         {return MULT;}
"("              {return LPAREN;}
")"              {return RPAREN;}
begin            {return Begin;}
end              {return End;}
print            {return PRINT;}
[0-9]+	         {return NUM;}
[A-Za-z"_"]+	 {yylval.id=strdup(yytext); return ID;}

.	 {printf("illegal token");}


