%{
#include <string.h>
%}

/*digit is just a shorthand/definition to be used when defining regular expressions*/
digit [0-9]

/*noyywrap option invoked*/
%option noyywrap


/*Regular expressions for generating tokens*/

%%

" " 		{continue;}
["/*"][A-Za-z0-9"_"" "]*["*/"] 	{continue;}
"var" 		{printf( "VAR\n");}
"type" 		{printf( "TYPE\n");}
"function"	{printf( "FUNCTION\n");}
"int"		{printf( "INT\n");}
"bool"		{printf( "BOOL\n");}
"if"		{printf( "IF\n");}
"else"		{printf( "ELSE\n");}
"for"		{printf( "FOR\n");}
"to"		{printf( "TO\n");}
"while"		{printf( "WHILE\n");}
"return"	{printf( "RETURN\n");}
"nil"		{printf( "NIL\n");}
"true"		{printf( "TRUE\n");}
"false"		{printf( "FALSE\n");}
"++"	{printf( "INCREMENT\n");}
"--" 	{printf( "DECREMENT\n");}
"+"		{printf( "PLUS\n");}
"-"		{printf( "MINUS\n");}
"/"		{printf( "MINUS\n");}
"%"		{printf( "MOD\n");}
"*"		{printf( "TIMES\n");}
"|"		{printf( "OR\n");}
"&"		{printf( "AND\n");}
"<="	{printf( "LESSEQUAL\n");}
">="	{printf( "GREATEREQUAL\n");}
"=="	{printf( "EQUAL\n");}
"!="	{printf( "NOTEQUAL\n");}
"<"		{printf( "LESS\n");}
">"		{printf( "GREATER\n");}
"="		{printf( "ASSIGN\n");}
"!"		{printf( "NOT\n");}
[A-Za-z][A-Za-z0-9"_"" "]*        {printf( "ID(%s)\n", yytext);}
{digit}+	               {printf( "NUM(%d)\n", atoi(yytext));}
"("		{printf( "LEFTPAREN\n");}
")"		{printf( "RIGHTTPAREN\n");}
"["		{printf( "LEFTSQUARE\n");}
"]"		{printf( "RIGHTSQUARE\n");}
"{"		{printf( "LEFTCURLY\n");}
"}"		{printf( "RIGHTCURLY\n");}
";"		{printf( "SEMICOLON\n");}
":"		{printf( "COLON\n");}
","		{printf( "COMMA\n");}
"."		{printf( "PERIOD\n");}
.	                       {printf( "illegal token(%s)\n", yytext);}
%%

/* Main program. Only needs to be here for standalone lexer*/

int main( argc, argv )
int argc;
char **argv;
{
    ++argv, --argc;   /*  skip over program name */
    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else 
            yyin = stdin;
         
    yylex();

}


