%{
#include <stdio.h>
int yylex (void);
void yyerror (char const *s) {
   fprintf (stderr, "%s\n", s);
 }
%}
%union{int num;
       char *id;
       }

%type <num> sList
%type <num> sListTail
%type <num> stmt

%token NUM <id>ID LPAREN RPAREN PRINT COLON EQ PLUS SEMICOLON MULT MINUS Begin End

%left PLUS MINUS
%left MULT
%left UMINUS

%start prog

%%

prog: Begin sList End {printf ("Number of assignments = %d\n", $2);} 


sList:  stmt SEMICOLON sListTail {$$= $1 + $3;}

sListTail:  stmt SEMICOLON  sListTail {$$= $1 + $3;}
            |                         {$$=0;}

stmt:  ID COLON EQ expr { printf("ID=%s\n", $1); $$ =1;}
     | PRINT LPAREN ID RPAREN {$$=0;}
     

expr: LPAREN expr RPAREN
      |   expr PLUS expr 
      |   expr MULT expr 
      |   expr MINUS expr
      |   MINUS expr   %prec UMINUS  
      |   PLUS  expr
      |   ID {printf("ID=%s\n", $1);}
      |   NUM   

%%
