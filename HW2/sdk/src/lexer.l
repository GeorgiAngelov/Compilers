%{

#include "tokens.h"

%}

%option noyywrap
%option nounput
%option noinput

%%

"var" {
      // A keyword.
      return VAR;
}

. {
      // Here we match any character that wasn't already matched by preceding
      // patterns.
      fprintf(stderr, "Error: unrecognized character: %s\n", yytext);
}

%%

// More C functions and such can go down here.

