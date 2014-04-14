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
      Type * Type;
      struct decl* decl;
      struct stmt* stmt;
      struct exp* exp;
      GList* GList;
      
      TypedId* typed_id;
      struct field_init* field_init;
      Type* type;
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

%type <decl> decl fun_decl type_decl var_decl
%type <exp> aexp exp bexp fun_call obj_lit lvalue
%type <stmt> stmt
%type <GList> var_decls field_decls param_decls decls block stmts exps
%type <type> type field_decl
%type <typed_id> param_decl

%start program

%%

program:
      decls {
        //done_parsing($1);
        //decls_print($1);
	  }

decls:									{GList temp; $$=&temp;}
      | decls decl						{GList * temp = g_list_append($1, $2); $$=temp;}

decl: var_decl | fun_decl | type_decl	{$$=$1;}

var_decls: 						{GList temp; $$=&temp;}
      | var_decls var_decl		{GList * temp = g_list_append($1, $2); 
      							$$=temp;}

var_decl: 
      T_VAR T_ID ':' type ';'				{
      										Symbol tempSym = symbol_var($2);
      										Type tempType = type_id(tempSym);
      										struct decl * temp = decl_new(tempSym, &tempType, NULL, NULL, NULL);
      										$$=temp;
      										}
      										
      | T_VAR T_ID ':' type '=' exp ';'		{
      										Symbol tempSym = symbol_var($2);
      										Type tempType = type_id(tempSym);
      										struct decl * temp = decl_new(tempSym, &tempType, $6, NULL, NULL);
      										$$=temp;
      										}

type_decl: 
      T_TYPE T_ID ':' type ';'				{
      											Symbol tempSym = symbol_typename($2);
      											struct decl * temp = decl_new(tempSym, $4, NULL, NULL, NULL);
      											$$= temp;
      										}

param_decl: 
      T_ID ':' type							{TypedId temp;
      										temp.id = symbol_field($1);//symbol
      										temp.type = $3;
      										$$= &temp;}

param_decls: 
      param_decl							{GList t; $$ = g_list_append(&t, $1);}
      | param_decls ',' param_decl			{GList * temp = g_list_append($1, $3); $$=temp;}

field_decl: 
      T_ID ':' type							{$$=type_new(*$3);}

field_decls: 
      field_decl						{GList t; $$=g_list_append(&t, $1);}
      | field_decls ',' field_decl		{GList * temp = g_list_append($1, $3);$$=temp;}

type: 
      T_INT						{Type temp = type_int(); $$=&temp;}
      | T_BOOL					{Type temp = type_bool(); $$=&temp;}
  	  | T_ID					{Type temp = type_id(symbol_typename($1)); $$=&temp;}
      | '[' type ']'			{Type temp = type_array($2); $$=&temp;}
      | '{' field_decls '}'		{Type temp = type_struct($2); $$=&temp;}

fun_decl: 
      T_FUNCTION T_ID '(' param_decls ')' ':' type '{' var_decls stmts '}'		{Type tempType = type_fun($4, $7);
      																			struct decl * temp = decl_new(symbol_fun($2), &tempType, NULL, $9, $10);
      																			$$=temp;}
      | T_FUNCTION T_ID '(' param_decls ')' '{' var_decls stmts '}'				{Type tempType = type_fun($4, NULL);
      																			struct decl * temp = decl_new(symbol_fun($2),&tempType, NULL, $7, $8);
      																			$$=temp;}
      | T_FUNCTION T_ID '(' ')' ':' type '{' var_decls stmts '}'				{Type tempType = type_fun(NULL, $6);
      																			struct decl * temp = decl_new(symbol_fun($2),&tempType, NULL, $8, $9);
      																			$$=temp;}
      | T_FUNCTION T_ID '(' ')' '{' var_decls stmts '}'							{Type tempType = type_fun(NULL, NULL);
      																			struct decl * temp = decl_new(symbol_fun($2), &tempType, NULL, $6, $7);
      																			$$=temp;}

exp:
      aexp | bexp						{$$=$1;}
      | obj_lit | fun_call | lvalue		{$$=$1;}
      | '(' exp ')'						{$$=$2;}

aexp: 
      T_NUM 						{struct exp* temp = exp_num_new($1);
      								$$=temp;}
      | '+' exp %prec T_UPLUS 		{struct exp* temp = exp_new(AST_EXP_PLUS);
      								temp->right = $2;
      								$$= temp;}
      | '-' exp %prec T_UMINUS 		{struct exp* temp = exp_new(AST_EXP_MINUS);
      								temp->right = $2;
      								$$= temp;}
      | exp '+' exp 				{struct exp* temp = exp_new(AST_EXP_PLUS);
      								temp->left = $1;
      								temp->right = $3;
      								$$= temp;}
      | exp '-' exp 				{struct exp* temp = exp_new(AST_EXP_MINUS);
      								temp->left = $1;
      								temp->right = $3;
      								$$= temp;}
      | exp '/' exp 				{struct exp* temp = exp_new(AST_EXP_DIV);
      								temp->left = $1;
      								temp->right = $3;
      								$$= temp;}
      | exp '%' exp 				{struct exp* temp = exp_new(AST_EXP_MOD);
      								temp->left = $1;
      								temp->right = $3;
      								$$= temp;}
      | exp '*' exp 				{struct exp* temp = exp_new(AST_EXP_MUL);
      								//exp_print(temp);
      								temp->left = $1;
      								temp->right = $3;
									$$ = temp;}

bexp: 
      T_TRUE			{struct exp * temp = exp_binop_new(AST_EXP_TRUE, NULL, NULL); 
      					$$= temp;}
      | T_FALSE			{struct exp * temp = exp_binop_new(AST_EXP_FALSE, NULL, NULL); 
      					$$= temp;}
      | '!' exp			{struct exp * temp = exp_binop_new(AST_EXP_NOT, NULL, $2); 
      					$$=temp;}
      | exp '|' exp		{struct exp * temp = exp_binop_new(AST_EXP_OR, $1, $3); 
      					$$=temp;}
      | exp '&' exp		{struct exp * temp = exp_binop_new(AST_EXP_AND, $1, $3); 
      					$$=temp;}
      | exp '<' exp		{struct exp * temp = exp_binop_new(AST_EXP_LT, $1, $3); 
      					$$=temp;}
      | exp "<=" exp	{struct exp * temp = exp_binop_new(AST_EXP_LT_EQ, $1, $3); 
      					$$=temp;}
      | exp '>' exp		{struct exp * temp = exp_binop_new(AST_EXP_GT, $1, $3); 
      					$$=temp;}
      | exp ">=" exp	{struct exp * temp = exp_binop_new(AST_EXP_GT_EQ, $1, $3); 
      					$$=temp;}
      | exp "==" exp	{struct exp * temp = exp_binop_new(AST_EXP_EQ, $1, $3); 
      					$$=temp;}
      | exp "!=" exp	{struct exp * temp = exp_binop_new(AST_EXP_NOT_EQ, $1, $3); 
      					$$=temp;}

obj_lit: array_lit | struct_lit
      | T_NIL

array_lit:
      '[' exps ']'
      | T_STR

exps: 
      exp					{GList temp = *g_list_append(&temp, $1);$$=&temp;}
      | exps ',' exp		{GList * temp = g_list_append($1, $3);$$=temp;}

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

stmts: 													{GList temp; $$=&temp;}
      | stmts stmt										{GList * temp = g_list_append($1, $2); $$=temp;}

stmt: 
      exp ';'											{struct stmt * temp = stmt_exp_new($1); $$=temp;}
      | lvalue '=' exp ';'								{struct stmt * temp = stmt_assign_new($1, $3); $$=temp;}
      | T_IF '(' exp ')' block T_ELSE block				{struct stmt * temp = stmt_if_new($3, $5, $7); $$=temp;}
      | T_IF '(' exp ')' block							{struct stmt * temp = stmt_if_new($3, $5, NULL); $$=temp;}
      | T_WHILE '(' exp ')' block						{struct stmt * temp = stmt_while_new($3, $5); $$=temp;}
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
