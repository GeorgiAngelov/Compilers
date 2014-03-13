#include <stdio.h>
#include "micro.tab.h"

int yyparse(void); 
extern FILE *yyin;




int main(int argc, char **argv) {
 char *fname; int tok;
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); return 0;}
 fname=argv[1];

 /*Use yyin to point to the file to be parsed*/
 yyin = fopen(fname,"r");
 if (!yyin) {"cannot open"; return 0;}
 yyparse();
 return 0;
}


