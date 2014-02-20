
all: lexer.o main.o
	g++ lexer.o main.o -o pa1
	rm lex.yy.cc

lexer.o: lex.yy.cc
	g++ -Wall -c -o lexer.o lex.yy.cc
	
main.o: 
	g++ -Wall -c -o main.o main.cpp
	
lex.yy.cc: 
	flex -+ mymicro.lex
	
clean: 
	rm *.o pa1 lex.yy.cc
	
	
	