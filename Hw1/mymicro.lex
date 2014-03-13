%{
#include <string.h>
#include <fstream>
#include <stack>
#include "utils.hpp"
%}

/*digit is just a shorthand/definition to be used when defining regular expressions*/
digit [0-9]

/*noyywrap option invoked*/
%option noyywrap
%option c++

/*Regular expressions for generating tokens*/

%%

" " 		{continue;}
"\n"		{continue;}
"/*"([^*]|("*"+([^*/])))*"*"+"/" {continue;}
"//"[^\r\n]* {continue;}
"var" 		{return VAR;}
"type" 		{return TYPE;}
"function"	{return FUNCTION;}
"int"		{return INT;}
"bool"		{return BOOL;}
"if"		{return IF;}
"else"		{return ELSE;}
"for"		{return FOR;}
"to"		{return TO;}
"while"		{return WHILE;}
"return"	{return RETURN;}
"nil"		{return NIL;}
"true"		{return TRUE;}
"false"		{return FALSE;}
"++"	{return INCREMENT;}
"--" 	{return DECREMENT;}
"+"		{return PLUS;}
"-"		{return MINUS;}
"/"		{return DIVISION;}
"%"		{return MOD;}
"\*"	{return TIMES;}
"|"		{return OR;}
"&"		{return AND;}
"<="	{return LESSEQUAL;}
">="	{return GREATEREQUAL;}
"=="	{return EQUAL;}
"!="	{return NOTEQUAL;}
"<"		{return LESS;}
">"		{return GREATER;}
"="		{return ASSIGN;}
"!"		{return NOT;}
[A-Za-z][A-Za-z0-9"_"]*        {return ID;}
{digit}+	               {return NUM;}
"("		{return LEFTPAREN;}
")"		{return RIGHTPAREN;}
"["		{return LEFTSQUARE;}
"]"		{return RIGHTSQUARE;}
"{"		{return LEFTCURLY;}
"}"		{return RIGHTCURLY;}
";"		{return SEMICOLON;}
\"(\\\"|\\\\|[^"\r\n\\])*\"   {return STRING;}
":"		{return COLON;}
","		{return COMMA;}
"."		{return PERIOD;}
[ \n\r\t]+ { continue; }
.	    {printf(" illegal token(%s)\n", yytext);}
%%

