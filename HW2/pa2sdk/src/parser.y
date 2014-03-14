%code top {

#include <stdio.h>
#include <string.h>
#include "parser.h"
#include <iostream>
#include <map>
#include <algorithm>    // std::sort
#include <vector>       // std::vector
using namespace std;

typedef struct fundata {
	char* name;
	int parity;
	int parity_mismatch;
	int references;
	int declared;
} funData;

// Defined in the C file generated by flex. We can't include lexer.h because
// that'd cause a circular dependency (we need the header generated by bison to
// generate the lexer.h header).
extern int yylex(void);
int result;
int eval = 0;
int beval = 0;

std::map<std::string, funData> function_map;

static void yyerror(const char*);

}

// The type of yylval.
%union {
      int val;
      int num;
      char* text;
      int boolean;
};

// These get stuck in a token enum in the header bison generates (parser.h),
// which we can then include in our lexer spec.
%token VAR TYPE FUNCTION
      FOR TO WHILE IF ELSE RETURN
      NIL TRUE FALSE
      INT <boolean>BOOL
      <text>ID STR <num>NUM
      UNKNOWN
      EVAL
      PRINT

// These associate tokens with string lits like "<=" so we can use them in our
// grammar.
%token LT_EQ "<="
%token GT_EQ ">="
%token EQ "=="
%token NOT_EQ "!="



%type <num> expr
%type <num> expr1
%type <num> expr2
%type <num> bexpr
%type <num> paramlist
%type <num> paramlist2
%type <num> exprlist
%type <num> exprList2
%type <num> P1
%type <num> P2

// The top-level rule.
%start program

%%

// A Liger program is either a list of declarations or it's an "extended Liger"
// program -- an EVAL token followed by a Liger expression.
program: stmtlist
      | EVAL '(' expr ')' ';'	{eval = 1;result = $3;}
      | EVAL '(' bexpr ')' ';'	{beval = 1;if ($3 == 1){result = 1;}else{result = 0;}}

stmtlist: stmt stmtlist
		|

stmt: decls
    |  FUNCTION ID '(' paramlist ')' returntype '{' stmtlist '}' 	{if (function_map.find($2) == function_map.end())
    																	{
    																		funData temp; 
    																		temp.name = $2; 
    																		temp.parity = $4; 
    																		temp.references = 0; 
    																		temp.declared = 1;
    																		function_map[$2] = temp;
    																	}
    																  else
    																  	{
    																  		function_map[$2].declared++;
    																  		//check parity if function was called but not declared yet
    																  		if ($4 != function_map[$2].parity)
																			{
																				function_map[$2].parity_mismatch = 1;
																			}
    																  	}
    																  }
	| IF '(' bexpr ')' '{' stmtlist '}' {printf ("IF statement\n");}
	| RETURN return_type ';' {/*printf("return\n");*/}
	| 

decls: VAR ID ':' DATA '=' expr ';' {printf ("Assignment with data\n");}
	|	VAR ID ':' DATA '=' array_assign ';' {printf ("Assignment with ARRAY\n");}
	|	PRINT '(' ID ')' 			{printf ("print\n");}
	|	VAR ID ':' '{' paramlist '}' ';'	{printf ("Structure\n");}
	|	VAR ID ':' DATA ';'			{printf ("Assignment without data\n");}
     
return_type:
			| '(' expr ')'
			|	expr

returntype: 
			| ':' DATA
			
exprlist: exprList2 		{$$=$1;}
exprList2:
		|	expr1 expr2		{$$= $1 + $2;}
expr1: expr 				{$$=1;}
expr2: 						{$$=0;}
		| ',' expr expr2	{$$=$3 + 1;}
		
expr: '(' expr ')'			{$$=$2;}
	|	NUM 				{$$=$1;}
	|	expr '+' expr 		{$$= $1 + $3;}
	|	expr '-' expr		{$$= $1 - $3;}
	|	expr '*' expr		{$$= $1 * $3;}
	|	expr '/' expr		{$$= $1 / $3;}
	|	expr '%' expr		{$$= $1 % $3;}
	|	ID '(' exprlist ')'	{
							if (function_map.find($1) == function_map.end())
    							{
    								funData temp; 
    								temp.name = $1; 
    								temp.parity = $3; 
    								temp.references = 1; 
    								temp.declared = 0;
    								function_map[$1] = temp;
								}
							else
								{
									function_map[$1].references++;
									//check parity
									if ($3 != function_map[$1].parity)
									{
										function_map[$1].parity_mismatch = 1;
									}
								}
							}
	|	ID					{}
	|	'-' NUM				{$$ = -1*$2;}
	|	'+' NUM				{$$ = $2;}



bexpr: TRUE					{$$= 1;}
	|	FALSE				{$$= 0;}
	|	bexpr '&' bexpr		{$$= $1 && $3;}
	|	bexpr '|' bexpr		{$$= $1 || $3;}
	| 	expr EQ expr		{$$= $1 == $3;}
	|	expr NOT_EQ expr	{$$= $1 != $3;}
	|	expr LT_EQ expr		{$$= $1 <= $3;}
	| 	expr GT_EQ expr		{$$= $1 >= $3;}
	|	expr '>' expr		{$$= $1 > $3;}
	|	expr '<' expr		{$$= $1 < $3;}
	| 	'!' bexpr			{$$= !$2;}

paramlist:	paramlist2

paramlist2:	{$$=0;}
			| P1 P2 {$$=$1 + $2;}
	
P1: ID ':' DATA {$$=1;}

P2: {$$=0;}
	| ','  ID ':' DATA P2 {$$= $5 + 1;}
	
DATA: INT
	| ARRAY
	| ID

ARRAY: '[' DATA ']'

array_assign: 
	|'[' array_data ']'

array_data: exprlist
		

val1:	ID {printf("val1 ID\n");}
	|	INT {printf("val1 INT\n");}
	|	array_assign  {printf("val1 ARRAY_ASSIGN\n");}

val2: ',' ID val2 {printf("val2 ID\n");}
	|	',' INT val2 {printf("val2 INT\n");}
	|	',' array_assign val2 {printf("val2 ARRAY_ASSIGN\n");}
	| {printf("val2 NULL\n");}
%%

void yyerror(const char* p) {
      fprintf(stderr, "%s\n", p);
}
