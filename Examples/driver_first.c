#include <stdio.h>
#include "tokens.h"


extern int yylex(void); 
extern FILE *yyin;



/*Creates an array of token names*/
char *toknames[] = {
"COLON", "EQ", "SEMICOLON", "LPAREN", "RPAREN", "PLUS", "MINUS", "BEGIN", "END", "PRINT", "ID", "NUM"
};



int main(int argc, char **argv) {
 char *fname; int tok;
 /*Check whether you spplied all the arguments*/
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); return 0;}
 fname=argv[1];
 
/* Use yyin to point to the file you need*/ 
 yyin = fopen(fname,"r");
 if (!yyin) {"cannot open"; return 0;}
 for(;;) 
 {
/* Call yylex to get the next token*/ 
   tok=yylex();
   if (tok==0) break;
   printf("This if coming from driver: %s\n",toknames[tok-259]);
 }
 return 0;
}


