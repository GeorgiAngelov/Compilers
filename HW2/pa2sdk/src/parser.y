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
int validResult = 1;

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
%type <num> paramlist
%type <num> paramlist2
%type <num> exprlist
%type <num> exprList2
%type <num> P1
%type <num> P2




%left '|'
%left '&'
%left EQ LT_EQ GT_EQ
%left NOT_EQ
%left '!'
%left '+' '-'
%left '*' '/' '%'
%left UMINUS

// The top-level rule.
%start program

%%

// A Liger program is either a list of declarations or it's an "extended Liger"
// program -- an EVAL token followed by a Liger expression.
program: stmtlist
      | EVAL '(' expr ')' ';'	{eval = 1;result = $3;}

stmtlist: 
	| stmt stmtlist

stmtlist_w_return : RETURN '(' expr ')' ';'
	| stmtlist RETURN '(' expr ')' ';'
		
stmt: decls
    |  FUNCTION ID '(' paramlist ')' func_right_side 	{if (function_map.find($2) == function_map.end())
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
	| IF '(' expr ')' '{' stmtlist '}' else_statement {/*printf ("IF statement\n");*/}
	| RETURN return_type ';' {/*printf("return\n");*/}
	| WHILE '(' expr ')' '{' stmtlist '}' {/*printf ("WHILE loop\n");*/}
	| FOR '(' ID '=' expr TO expr ')' '{' stmtlist '}'

func_right_side: ':' INT '{' stmtlist_w_return '}'
	|	'{' stmtlist '}'
	
else_statement:
	| ELSE '{' stmtlist '}'

decls: VAR ID ':' DATA '=' expr ';' {/*printf ("Assignment with data\n");*/} 
	|	VAR ID ':' DATA '=' array_assign ';' {/*printf ("Assignment with ARRAY\n");*/}
	|	PRINT '(' ID ')' ';'			{/*printf ("print\n");*/}
	|	VAR ID ':' '{' paramlist '}' ';'	{/*printf ("Structure\n");*/}
	|	VAR ID ':' DATA ';'			{/*printf ("Assignment without data\n");*/}
	|	TYPE ID ':' '{' struct_declare '}' ';'
	|	VAR ID ':' ID '=' '{' struct_declare '}' ';'
	|	VAR ID ':' '{' paramlist '}' '=' '{'struct_declare '}' ';'
	|	'{' struct_declare '}' '.' ID '=' expr ';'					{validResult = 0;}
	|	func_left_side func_right_side ';'
	|	array_assign '[' NUM ']' '=' expr ';'
	|	ID '=' expr ';'

func_left_side: ID '(' exprlist ')'	{
					//if the function has not been previously encountered
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
	
func_right_side:
	|	'[' NUM ']' '=' expr 
	|	'.' ID '=' expr			{validResult = 0;}
	
struct_declare: ID ':' INT struct_declare2
	|	ID '=' expr struct_declare2
	|	ID '='  '{'struct_declare '}' struct_declare2

struct_declare2: 
	|	',' ID ':' INT struct_declare2
	|	',' ID '=' expr struct_declare2
	|	',' ID '=' '{' struct_declare '}' struct_declare2

return_type:
			| '(' expr ')'
			|	expr

returntype: ':' DATA
			
exprlist: exprList2 		{$$=$1;}
exprList2: expr1 expr2		{$$= $1 + $2;}
			|				{$$=0;}	
expr1: expr 				{$$=1;}
expr2: 						{$$=0;}
		| ',' expr expr2	{$$=$3 + 1;}
		
expr: '(' expr ')'			{$$=$2;}
	|	NUM 				{$$=$1;}
	|	expr '+' expr 		{$$= $1 + $3;}
	|	expr '-' expr		{$$= $1 - $3;}
	|	expr '*' expr		{$$= $1 * $3;}
	|	expr '/' expr			{if ($3 == 0){validResult = 0; $$=0;}else{$$= $1 / $3;}}
	|	expr '%' expr			{if ($3 == 0){validResult = 0; $$=0;}else{$$= $1 % $3;}}
	|	func_left_side
	|	ID					{}
	|	'-' expr			{$$ = -1*$2;}
	|	'+' expr			{$$ = $2;}
	|	array_assign		{}
	|	'{' struct_declare '}' '.' ID		{validResult = 0;}
	| 	TRUE				{beval = 1;$$= 1;}
	|	FALSE				{beval = 1;$$= 0;}
	|	expr '&' expr		{beval = 1;$$= $1 && $3;}
	|	expr '|' expr		{beval = 1;$$= $1 || $3;}
	| 	expr EQ expr		{beval = 1;$$= $1 == $3;}
	|	expr NOT_EQ expr	{beval = 1;$$= $1 != $3;}
	|	expr LT_EQ expr		{beval = 1;$$= $1 <= $3;}
	| 	expr GT_EQ expr		{beval = 1;$$= $1 >= $3;}
	|	expr '>' expr		{beval = 1;$$= $1 > $3;}
	|	expr '<' expr		{beval = 1;$$= $1 < $3;}
	| 	'!' expr			{beval = 1;$$= !$2;}

paramlist:	paramlist2

paramlist2:	{$$=0;}
			| P1 P2 {$$=$1 + $2;}
	
P1: ID ':' DATA {$$=1;}

P2: {$$=0;}
	| ','  ID ':' DATA P2 {$$= $5 + 1;}
	
DATA: INT
	| ARRAY
	| ID
	| struct_declare
	| array_assign

ARRAY: '[' DATA ']'

array_assign: '[' exprlist ']'

%%

void yyerror(const char* p) {
      fprintf(stderr, "%s\n", p);
}
