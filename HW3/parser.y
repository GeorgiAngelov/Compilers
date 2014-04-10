%code top {

#include "frontend.h"
#include "type.h"
#include "ast.h"
#include "env.h"
#include <glib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct fun_info {
      char* name;
      int arity_mismatch;
      int arity;
      int ndefs;
      int ncalls;
};

extern int yylex(void);
extern int yyerror(const char*);

}

%code requires {

#include "type.h"
#include "ast.h"
#include <glib.h>

}

%union {
      char* id;
      char* str;
      int num;
      struct stmt* stmt;
      struct exp* exp;
      GList* GList;
};


/* Ids could be types or exps. */
%token <id> T_ID
%token <str> T_STR
%token <num> T_NUM

%token T_LT_EQ "<="
%token T_GT_EQ ">="
%token T_EQ "=="
%token T_NOT_EQ "!="

%token T_VAR T_TYPE T_FUNCTION
      T_FOR T_TO T_WHILE T_IF T_ELSE T_RETURN
      T_NIL T_TRUE T_FALSE
      T_INT T_BOOL
      T_UNKNOWN

%left '|'
%left '&'
%left "==" "!="
%left '<' '>' "<=" ">="
%left '+' '-'
%left '*' '/' '%'
%left T_UMINUS T_UPLUS '!'

%type <exp> aexp exp bexp fun_call obj_lit
%type <stmt> stmt
%type <exp> lvalue
%type <GList> block stmts

%start program

%%

program:
      decls {
        //done_parsing($1);
	  }

decls:
      | decls decl

decl: var_decl | fun_decl | type_decl

var_decls: 
      | var_decls var_decl

var_decl: 
      T_VAR T_ID ':' type ';'
      | T_VAR T_ID ':' type '=' exp ';'

type_decl: 
      T_TYPE T_ID ':' type ';'

param_decl: 
      T_ID ':' type

param_decls: 
      param_decl
      | param_decls ',' param_decl

field_decl: 
      T_ID ':' type

field_decls: 
      field_decl
      | field_decls ',' field_decl

type: 
      T_INT
      | T_BOOL
      | T_ID
      | '[' type ']'
      | '{' field_decls '}'

fun_decl: 
      T_FUNCTION T_ID '(' param_decls ')' ':' type '{' var_decls stmts '}'
      | T_FUNCTION T_ID '(' param_decls ')' '{' var_decls stmts '}'
      | T_FUNCTION T_ID '(' ')' ':' type '{' var_decls stmts '}'
      | T_FUNCTION T_ID '(' ')' '{' var_decls stmts '}'

exp:
      aexp | bexp						{$$=$1;}
      | obj_lit | fun_call | lvalue		{$$=$1;}
      | '(' exp ')'						{$$=$2;}

aexp: 
      T_NUM {/*
		struct exp* tmp = exp_num_new($1);
	  */}
      | '+' exp %prec T_UPLUS {
		struct exp* temp = exp_new(AST_EXP_PLUS);
		$$= temp;
	  }
      | '-' exp %prec T_UMINUS {
		struct exp* temp = exp_new(AST_EXP_MINUS);
		$$= temp;
	  }
      | exp '+' exp {struct exp* temp = exp_new(AST_EXP_PLUS);$$= temp;}
      | exp '-' exp {struct exp* temp = exp_new(AST_EXP_MINUS);$$= temp;}
      | exp '/' exp {struct exp* temp = exp_new(AST_EXP_DIV);$$= temp;}
      | exp '%' exp {struct exp* temp = exp_new(AST_EXP_MOD);$$= temp;}
      | exp '*' exp {
		struct exp* temp = exp_new(AST_EXP_MUL);
		exp_print(temp);
		temp->left = $1;
		temp->right = $3;
		$$ = temp;
		}

bexp: 
      T_TRUE			{struct exp * temp = exp_binop_new(AST_EXP_TRUE, NULL, NULL); $$= temp;}
      | T_FALSE			{struct exp * temp = exp_binop_new(AST_EXP_FALSE, NULL, NULL); $$= temp;}
      | '!' exp			{struct exp * temp = exp_binop_new(AST_EXP_NOT, NULL, $2); $$=temp;}
      | exp '|' exp		{struct exp * temp = exp_binop_new(AST_EXP_OR, $1, $3); $$=temp;}
      | exp '&' exp		{struct exp * temp = exp_binop_new(AST_EXP_AND, $1, $3); $$=temp;}
      | exp '<' exp		{struct exp * temp = exp_binop_new(AST_EXP_LT, $1, $3); $$=temp;}
      | exp "<=" exp	{struct exp * temp = exp_binop_new(AST_EXP_LT_EQ, $1, $3); $$=temp;}
      | exp '>' exp		{struct exp * temp = exp_binop_new(AST_EXP_GT, $1, $3); $$=temp;}
      | exp ">=" exp	{struct exp * temp = exp_binop_new(AST_EXP_GT_EQ, $1, $3); $$=temp;}
      | exp "==" exp	{struct exp * temp = exp_binop_new(AST_EXP_EQ, $1, $3); $$=temp;}
      | exp "!=" exp	{struct exp * temp = exp_binop_new(AST_EXP_NOT_EQ, $1, $3); $$=temp;}

obj_lit: array_lit | struct_lit
      | T_NIL

array_lit:
      '[' exps ']'
      | T_STR

exps: 
      exp
      | exps ',' exp

struct_lit:
      '{' field_inits '}'

field_init: 
      T_ID '=' exp

field_inits: 
      field_init
      | field_inits ',' field_init

fun_call:
      T_ID '(' exps ')'
      | T_ID '(' ')'

lvalue: 
      T_ID
      | struct_exp '.' T_ID
      | array_exp '[' exp ']'

array_exp: array_lit | fun_call | lvalue
struct_exp: struct_lit | fun_call | lvalue

stmts: 
      | stmts stmt										{GList * temp = g_list_append($1, $2); $$=temp;}

stmt: 
      exp ';'											{struct stmt * temp = stmt_exp_new($1); $$=temp;}
      | lvalue '=' exp ';'								{struct stmt * temp = stmt_assign_new($1, $3); $$=temp;}
      | T_IF '(' exp ')' block T_ELSE block				{struct stmt * temp = stmt_if_new($3, $5, $7); $$=temp;}
      | T_IF '(' exp ')' block							{struct stmt * temp = stmt_if_new($3, $5, NULL); $$=temp;}
      | T_WHILE '(' exp ')' block						{struct stmt * temp = stmt_while_new($3, $5);}
      | T_FOR '(' lvalue '=' exp T_TO exp ')' block		{struct stmt * temp = stmt_for_new($3, $5, $7, $9); $$=temp;}
      | T_RETURN '(' exp ')' ';'						{struct stmt * temp = stmt_return_new($3); $$=temp;}
      | T_RETURN ';'									{struct stmt * temp = stmt_return_new(NULL); $$=temp;}

block:
      '{' stmts '}'										{$$=$2;}

%%

int yyerror(const char *p) {
      fprintf(stderr, "Error: %s\n", p);
      return 0;
}
