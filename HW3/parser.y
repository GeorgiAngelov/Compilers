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
%type <exp> aexp exp bexp fun_call obj_lit lvalue struct_exp array_exp struct_lit array_lit
%type <stmt> stmt
%type <GList> var_decls field_decls param_decls decls block stmts exps field_inits
%type <type> type field_decl
%type <typed_id> param_decl
%type <field_init> field_init

%start program

%%

program:
      decls {
            //done_parsing($1);
            decls_print($1);
	}

decls:							{$$=NULL;}
      | decls decl					{$$ = g_list_append($1, $2);}

decl: var_decl | fun_decl | type_decl		{$$=$1;}

var_decls: 							{$$=NULL;}
      | var_decls var_decl				{$$ = g_list_append($1, $2);}

var_decl: 
      T_VAR T_ID ':' type ';'				{$$ = decl_new(symbol_var($2), $4, NULL, NULL, NULL);}
      										
      | T_VAR T_ID ':' type '=' exp ';'		{$$ = decl_new(symbol_var($2), $4, $6, NULL, NULL);}

type_decl: 
      T_TYPE T_ID ':' type ';'			{$$ = decl_new(symbol_typename($2), $4, NULL, NULL, NULL);}

param_decl: 
      T_ID ':' type					{$$ = typed_id_new(typed_id(symbol_field($1), $3));}

param_decls: 
      param_decl						{$$ = g_list_append(NULL, $1);}
      | param_decls ',' param_decl			{$$ = g_list_append($1, $3);}

field_decl:                                          
      T_ID ':' type					{$$=$3;}

field_decls:                                    
      field_decl						{$$ = g_list_append(NULL, $1);}
      | field_decls ',' field_decl			{$$ = g_list_append($1, $3);}

type:                                           
      T_INT							{$$=type_new(type_nil());}
      | T_BOOL						{$$=type_new(type_bool());}
  	| T_ID						{$$=type_new(type_id(symbol_typename($1)));}
      | '[' type ']'				      {$$=type_new(type_array($2));}
      | '{' field_decls '}'				{$$=type_new(type_struct($2));}

fun_decl: 
      T_FUNCTION T_ID '(' param_decls ')' ':' type '{' var_decls stmts '}'		{$$ = decl_new(symbol_fun($2), type_new(type_fun($4, $7)), NULL, $9, $10);}
      | T_FUNCTION T_ID '(' param_decls ')' '{' var_decls stmts '}'			{$$ = decl_new(symbol_fun($2), type_new(type_fun($4, NULL)), NULL, $7, $8);}
      | T_FUNCTION T_ID '(' ')' ':' type '{' var_decls stmts '}'				{$$ = decl_new(symbol_fun($2), type_new(type_fun(NULL, $6)), NULL, $8, $9);}
      | T_FUNCTION T_ID '(' ')' '{' var_decls stmts '}'                             {$$ = decl_new(symbol_fun($2), type_new(type_fun(NULL, NULL)), NULL, $6, $7);}

exp:
      aexp | bexp						{$$=$1;}
      | obj_lit | fun_call | lvalue			{$$=$1;}
      | '(' exp ')'					{$$=$2;}

aexp: 
      T_NUM 						{$$=exp_num_new($1);}
      | '+' exp %prec T_UPLUS 			{$$ = exp_binop_new(AST_EXP_PLUS, NULL, $2);}
      | '-' exp %prec T_UMINUS 			{$$ = exp_binop_new(AST_EXP_MINUS, NULL, $2);}
      | exp '-' exp 					{$$ = exp_binop_new(AST_EXP_MINUS, $1, $3);}
      | exp '/' exp 					{$$ = exp_binop_new(AST_EXP_DIV, $1, $3);}
      | exp '%' exp 					{$$ = exp_binop_new(AST_EXP_MOD, $1, $3);}
      | exp '*' exp 					{$$ = exp_binop_new(AST_EXP_MUL, $1, $3);}
      | exp '+' exp                             {$$ = exp_binop_new(AST_EXP_PLUS, $1, $3);} //This line was manually added

bexp: 
      T_TRUE						{$$ = exp_binop_new(AST_EXP_TRUE, NULL, NULL);}
      | T_FALSE						{$$ = exp_binop_new(AST_EXP_FALSE, NULL, NULL);}
      | '!' exp						{$$ = exp_binop_new(AST_EXP_NOT, NULL, $2);}
      | exp '|' exp					{$$ = exp_binop_new(AST_EXP_OR, $1, $3);}
      | exp '&' exp					{$$ = exp_binop_new(AST_EXP_AND, $1, $3);}
      | exp '<' exp					{$$ = exp_binop_new(AST_EXP_LT, $1, $3);}
      | exp "<=" exp					{$$ = exp_binop_new(AST_EXP_LT_EQ, $1, $3);}
      | exp '>' exp					{$$ = exp_binop_new(AST_EXP_GT, $1, $3);}
      | exp ">=" exp					{$$ = exp_binop_new(AST_EXP_GT_EQ, $1, $3);}
      | exp "==" exp					{$$ = exp_binop_new(AST_EXP_EQ, $1, $3);}
      | exp "!=" exp					{$$ = exp_binop_new(AST_EXP_NOT_EQ, $1, $3);}

obj_lit: array_lit | struct_lit			{$$=$1;}
      | T_NIL						{$$= exp_new(AST_EXP_NIL);}
	
array_lit:									
      '[' exps ']'					{$$= exp_array_lit_new($2);}
      | T_STR						{$$= exp_str_new($1);}
	
exps: 	
      exp							{$$ = g_list_append(NULL, $1);}
      | exps ',' exp					{$$ = g_list_append($1, $3);}
	
struct_lit:									
      '{' field_inits '}'				{$$ = exp_struct_lit_new($2);}
	
field_init: 								
      T_ID '=' exp					{$$= field_init_new(symbol_field($1), $3);}
	
field_inits: 								
      field_init						{$$ = g_list_append(NULL, $1);}
      | field_inits ',' field_init			{$$ = g_list_append($1, $3);}
	
fun_call:	
      T_ID '(' exps ')'					{$$= exp_fun_call_new(symbol_fun($1) ,$3);}
      | T_ID '(' ')'					{$$= exp_fun_call_new(symbol_fun($1), NULL);}

lvalue: //returns an expr *
      T_ID							{$$= exp_id_new(symbol_var($1));}
      | struct_exp '.' T_ID				{$$= exp_field_lookup_new($1, symbol_field($3));}
      | array_exp '[' exp ']'				{$$= exp_array_index_new($1, $3);}

array_exp: array_lit | fun_call | lvalue		{$$=$1;}
struct_exp: struct_lit | fun_call | lvalue	{$$=$1;}

stmts: 							{$$=NULL;}
      | stmts stmt					{$$ = g_list_append($1, $2);}

stmt:                                                       
      exp ';'								{$$ = stmt_exp_new($1);}
      | lvalue '=' exp ';'						{$$ = stmt_assign_new($1, $3);}
      | T_IF '(' exp ')' block T_ELSE block			{$$ = stmt_if_new($3, $5, $7);}
      | T_IF '(' exp ')' block					{$$ = stmt_if_new($3, $5, NULL);}
      | T_WHILE '(' exp ')' block					{$$ = stmt_while_new($3, $5);}
      | T_FOR '(' lvalue '=' exp T_TO exp ')' block		{$$ = stmt_for_new($3, $5, $7, $9);}
      | T_RETURN '(' exp ')' ';'					{$$ = stmt_return_new($3);}
      | T_RETURN ';'							{$$ = stmt_return_new(NULL);}

block:
      '{' stmts '}'					{$$=$2;}

%%

int yyerror(const char *p) {
      fprintf(stderr, "Error: %s\n", p);
      return 0;
}
