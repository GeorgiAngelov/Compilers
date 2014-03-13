#include "lexer.h" // yyin, yylex, etc.
#include "parser.h" // yyparse, yylval, token enum, etc.

int main(int argc, char** argv) {
	
	if(argc!=2){
		printf("To run please execute: ./pa2 <file_name>\n");
		exit(-1);
	}
	
	FILE *f = fopen(argv[1], "r");
	if(!f){
		printf("File could not be opened\n");
		exit(-1);
	}
	//p
	yyrestart(f);
    yyparse();
    return 0;
}