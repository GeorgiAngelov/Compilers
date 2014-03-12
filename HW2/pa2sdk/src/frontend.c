#include "lexer.h" // yyin, yylex, etc.
#include "parser.h" // yyparse, yylval, token enum, etc.

int main(int argc, char** argv) {
    yyparse();
      return 0;
}