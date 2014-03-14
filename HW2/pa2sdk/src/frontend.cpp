#include "lexer.h" // yyin, yylex, etc.
#include "parser.h" // yyparse, yylval, token enum, etc.
#include <unordered_map>
#include <iostream>

typedef struct fundata {
	char* name;
	int parity;
	int parity_mismatch;
	int references;
	int declared;
} funData;

extern int result;
extern std::unordered_map<std::string, funData> function_map;

using namespace std;

int main(int argc, char** argv) {
	int check;
	int eval = 1;
	
	if(argc!=2){
		printf("To run please execute: ./pa2 <file_name>\n");
		exit(-1);
	}
	
	FILE *f = fopen(argv[1], "r");
	if(!f){
		printf("File could not be opened\n");
		exit(-1);
	}
	
	yyrestart(f);
    check = yyparse();
    
    cout << "Valid Liger: ";
    
    if (check == 1)
    {
    	cout << "yes\n"; 
    }
    else
    {
    	cout << "no\n";
    }
    
    if (eval == 1)
    cout << "Result: " << result << "\n";
    
    
    for (auto it = function_map.begin(); it != function_map.end(); ++it)
    {
    	cout << "Function " << it->second.name << ":\n";
    	if (it->second.parity_mismatch == 1)
    	{
    		cout << "   Arity mismatch!\n";
    	}
    	else
    	{
    		cout << "   Arity: " << it->second.parity << "\n";
    	}
    	if (it->second.declared > 1)
    	{
    		cout <<	"   Multiple definitions!\n";
    	}
    	else
    	{
    		cout << "   Defined: " << it->second.declared << "\n";
    	}
    	cout << "   Calls: " << it->second.references << "\n";
    }
    
    
    return 0;
}