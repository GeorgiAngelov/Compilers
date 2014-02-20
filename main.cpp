using namespace std;
#include <string.h>
#include <fstream>
#include <stack>
#include "utils.hpp"
#include <FlexLexer.h>

/* Main program. Only needs to be here for standalone lexer*/

int main( int argc, char* argv[] )
{
    ifstream LexerFIn;
    LexerFIn.open(argv[1]);
	yyFlexLexer lexer;
	int token;
	int counter=0;
	stack<int> tok_stack;
	int lone_right = 0;
	int main_declared = 0;
	/**
	 *
	 *
	 **/
	while((token = lexer.yylex()) != 0){
		counter++;
		switch(token){
			case LEFTPAREN:
				tok_stack.push(LEFTPAREN);
			break;
			case LEFTSQUARE:
				tok_stack.push(LEFTSQUARE);
			break;
			case LEFTCURLY:
				tok_stack.push(LEFTCURLY);
			break;
			case RIGHTPAREN:
				if(tok_stack.size() == 0) lone_right = 1;
				else if(tok_stack.top() == LEFTPAREN)
					tok_stack.pop();
			break;
			case RIGHTSQUARE:
				if(tok_stack.size() == 0) lone_right = 1;
				else if(tok_stack.top() == LEFTSQUARE)
					tok_stack.pop();
			break;
			case RIGHTCURLY:
				if(tok_stack.size() == 0) lone_right = 1;
				else if(tok_stack.top() == LEFTCURLY)
					tok_stack.pop();
			break;
			default:
			{
				if (token == ID && strcmp(lexer.YYText(),"main")==0)
				{
					main_declared = 1;
				}
			}
		}
	};
		
	cout << "Total tokens: " << counter << "\n";
	
	cout << "Main declared: ";
	if(main_declared)
		cout << "Yes\n";
	else
		cout << "No\n";
	
	cout << "All brackets match: ";
	if(tok_stack.size() > 0 || lone_right == 1)
		 cout << "No\n";
	else{
		cout << "Yes\n";
	}

	//lexer.yylex();
	return 0;
}