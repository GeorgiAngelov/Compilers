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

%type <struct exp*> aexp exp bexp

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
      aexp | bexp
      | obj_lit | fun_call | lvalue
      | '(' exp ')'

aexp: 
      T_NUM {
		struct exp* tmp = exp_num_new($1);
	  }
      | '+' exp %prec T_UPLUS {
		struct exp* tmp = exp_new(AST_EXP_PLUS);
	  }
      | '-' exp %prec T_UMINUS {
		struct exp* tmp = exp_new(AST_EXP_MINUS);
	  }
      | exp '+' exp {struct exp* tmp = exp_new(AST_EXP_PLUS);}
      | exp '-' exp {struct exp* tmp = exp_new(AST_EXP_MINUS);}
      | exp '/' exp {struct exp* tmp = exp_new(AST_EXP_DIV);}
      | exp '%' exp {struct exp* tmp = exp_new(AST_EXP_MOD);}
      | exp '*' exp {
		struct exp* tmp = exp_new(AST_EXP_MUL);
		exp_print(tmp);
		/*tmp->left = $1;
		tmp->right = $3;
		$$ = tmp;*/
		}

bexp: 
      T_TRUE
      | T_FALSE
      | '!' exp
      | exp '|' exp
      | exp '&' exp
      | exp '<' exp
      | exp "<=" exp
      | exp '>' exp
      | exp ">=" exp
      | exp "==" exp
      | exp "!=" exp

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
      | stmts stmt

stmt: 
      exp ';'
      | lvalue '=' exp ';'
      | T_IF '(' exp ')' block T_ELSE block
      | T_IF '(' exp ')' block
      | T_WHILE '(' exp ')' block
      | T_FOR '(' lvalue '=' exp T_TO exp ')' block
      | T_RETURN '(' exp ')' ';'
      | T_RETURN ';'

block:
      '{' stmts '}'

%%

int yyerror(const char *p) {
      fprintf(stderr, "Error: %s\n", p);
      return 0;
}
