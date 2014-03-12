%{
#include <string.h>
%}

/*digit is just a shorthand/definition to be used when defining regular expressions*/
digit [0-9]

/*noyywrap option invoked*/
%option noyywrap


/*Regular expressions for generating tokens*/

%%

" "          	               {continue;}
"\n"	                       {continue;}
";"	                       {printf( "SEMICOLON\n");}
":"	                       {printf( "COLON\n");}
"="	                       {printf( "EQ\n");}
"("                            {printf( "LPAREN\n");}
")"                            {printf( "RPAREN\n");}
"+"                            {printf( "PLUS\n");}
"-"                            {printf( "MINUS\n");}
"begin"	                       {printf( "BEGIN\n");}
"end"	                       {printf( "END\n");}
"print"	                       {printf( "PRINT\n");}
{digit}+	               {printf( "NUM(%d)\n", atoi(yytext));}
[A-Za-z][A-Za-z0-9"_"]*        {printf( "ID(%s)\n", yytext);}	 
.	                       {printf( "illegal token(%s)\n", yytext);}
%%

/* Main program. Only needs to be here for standalone lexer*/

int main( argc, argv )
int argc;
char **argv;
{
    ++argv, --argc;   /*  skip over program name */
    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else 
            yyin = stdin;
         
    yylex();

}


