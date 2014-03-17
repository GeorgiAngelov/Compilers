#include "lexer.h" // yyin, yylex, etc.
#include "parser.h" // yyparse, yylval, token enum, etc.
#include <map>
#include <vector>       // std::vector
#include <iostream>

typedef struct fundata {
	char* name;
	int parity;
	int parity_mismatch;
	int references;
	int declared;
	int returnReq;
	int returnCount;
} funData;

extern int result;
extern int eval;
extern int beval;
extern int ieval;
extern int validResult;
extern std::vector<std::string> function_name;
extern std::map<std::string, funData> function_map;

using namespace std;

int main(int argc, char** argv) {
	int check;
	int validReturns = 1;
	
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
    
    /*
    for (auto it = function_map.begin(); it != function_map.end(); ++it)
    {
    	if (it->second.returnReq && it->second.returnCount < 1)
    	{
    		validReturns = 0;	
    	}
    }
    */
    
    
    
    
    cout << "Genuine Liger: ";
    
    if (check == 1)
    {
    	cout << "no\n"; 
    }
    else
    {
    	cout << "yes\n";
	    if (eval == 1)
	    {
		    if (validResult == 1)
		    {
		    	if (beval == 1)
		    	{
			    	cout << "Result: ";
			 		if (result == 1)
		    		cout << "true\n";
		    		else
		    		cout << "false\n";
		    	}
		    	else if (eval==1)
		    	cout << "Result: " << result << "\n";
		    }
		    else
		    {
		    	cout << "Result: unknown\n";
		    }
	    }
	    
	    //sort(function_map.begin(), function_map.end());
	    
	    if (eval == 0)
	    {
		    for (int i = 0; i < function_name.size(); ++i)
		    {
		    	cout << "Function " << function_map[function_name[i]].name << ":\n";
		    	if (function_map[function_name[i]].parity_mismatch == 1)
		    	{
		    		cout << "   Arity mismatch!\n";
		    	}
		    	else
		    	{
		    		cout << "   Arity: " << function_map[function_name[i]].parity << "\n";
		    	}
		    	if (function_map[function_name[i]].declared > 1)
		    	{
		    		cout <<	"   Multiple definitions!\n";
		    	}
		    	else if (function_map[function_name[i]].declared == 1)
		    	{
		    		cout << "   Defined: yes" << "\n";
		    	}
		    	else 
		    	{
		    		cout << "   Defined: no" << "\n";	
		    	}
		    	cout << "   Calls: " << function_map[function_name[i]].references << "\n";
		    }
	    }
    }
    
    return 0;
}