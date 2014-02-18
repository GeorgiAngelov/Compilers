%{
#include <string.h>
#include <fstream>
#include <stack>
#include "utils.hpp"
using namespace std;
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
"/""\*".*"\*""/"	{continue;}
"//".*"\n"	{continue;}
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
":"		{return COLON;}
","		{return COMMA;}
"."		{return PERIOD;}
.	    {printf(" illegal token(%s)\n", yytext);}
%%

/* Main program. Only needs to be here for standalone lexer*/

int main( int argc, char* argv[] )
{
    ifstream LexerFIn;
    LexerFIn.open(argv[1]);
    yyFlexLexer lexer;
	int token;
	int counter=0;
	stack<int> tok_stack;
	int lone_right = 0;
	int main_declared = 0;
	/**
	 *
	 *
	 **/
	while((token = lexer.yylex()) != 0){
		counter++;
		switch(token){
			case LEFTPAREN:
				tok_stack.push(LEFTPAREN);
			break;
			case LEFTSQUARE:
				tok_stack.push(LEFTSQUARE);
			break;
			case LEFTCURLY:
				tok_stack.push(LEFTCURLY);
			break;
			case RIGHTPAREN:
				if(tok_stack.size() == 0) lone_right = 1;
				else if(tok_stack.top() == LEFTPAREN)
					tok_stack.pop();
			break;
			case RIGHTSQUARE:
				if(tok_stack.size() == 0) lone_right = 1;
				else if(tok_stack.top() == LEFTSQUARE)
					tok_stack.pop();
			break;
			case RIGHTCURLY:
				if(tok_stack.size() == 0) lone_right = 1;
				else if(tok_stack.top() == LEFTCURLY)
					tok_stack.pop();
			break;
			default:
			{
				if (token == ID && strcmp(lexer.YYText(),"main")==0)
				{
					main_declared = 1;
				}
			}
		}
	};
		
	cout << "Total tokens: " << counter << "\n";
	
	cout << "Main declared: ";
	if(main_declared)
		cout << "Yes\n";
	else
		cout << "No\n";
	
	cout << "All brackets match: ";
	if(tok_stack.size() > 0 || lone_right == 1)
		 cout << "No\n";
	else{
		cout << "Yes\n";
	}

	//lexer.yylex();
	return 0;
}


