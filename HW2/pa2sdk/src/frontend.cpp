#include "lexer.h" // yyin, yylex, etc.
#include "parser.h" // yyparse, yylval, token enum, etc.
#include <map>
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <iostream>

typedef struct fundata {
	char* name;
	int parity;
	int parity_mismatch;
	int references;
	int declared;
} funData;

extern int result;
extern int eval;
extern int beval;
extern int validResult;
extern std::map<std::string, funData> function_map;

using namespace std;

int main(int argc, char** argv) {
	int check;
	
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
    
    cout << "Genuine Liger: ";
    
    if (check == 0)
    {
    	cout << "yes\n"; 
    }
    else
    {
    	cout << "no\n";
    }
    
    if (validResult == 1)
    {
    	if (eval == 1)
    	cout << "Result: " << result << "\n";
    	
    	if (beval == 1)
    	{
	    	cout << "Result: ";
	 		if (result == 1)
    		cout << "true\n";
    		else
    		cout << "false\n";
    	}
    }
    else
    {
    	cout << "Result: unknown\n";
    }
    
    //sort(function_map.begin(), function_map.end());
    
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
    	else if (it->second.declared == 1)
    	{
    		cout << "   Defined: yes" << "\n";
    	}
    	else 
    	{
    		cout << "   Defined: no" << "\n";	
    	}
    	cout << "   Calls: " << it->second.references << "\n";
    }
    
    
    return 0;
}