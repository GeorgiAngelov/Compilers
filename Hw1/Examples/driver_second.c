#include <stdio.h>
#include "tokens.h"

extern int yylex(void); 
extern FILE *yyin;



char *toknames[] = {
"COLON", "EQ", "SEMICOLON", "LPAREN", "RPAREN", "PLUS", "MINUS", "BEGIN", "END", "PRINT", "ID", "NUM"
};



int main(int argc, char **argv) {
 char *fname; int tok;
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); return 0;}
 fname=argv[1];

 /*Use yyin to point to the file to be lexed*/
 yyin = fopen(fname,"r");
 if (!yyin) {"cannot open"; return 0;}
 for(;;) 
 {
   /*Use yylex to get the next token*/
   tok=yylex();
   if (tok==0) break;
   
   /*Use yylval to get the tokens*/ 
   switch(tok) 
   {
   case ID: 
         { printf("%s(%s)\n",toknames[tok-259], yylval.idval);
           break;}
   case NUM: 
         { printf("%s(%i)\n",toknames[tok-259], yylval.numval);
           break;}  
   default:   
         { printf("%s\n",toknames[tok-259]);
           break;}
   }
 }
 return 0;
}


