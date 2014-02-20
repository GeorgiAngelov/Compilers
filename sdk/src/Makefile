
all: lexer.o frontend.o
	gcc lexer.o frontend.o -o pa1
	rm lexer.c

lexer.o: lexer.c
	gcc -Wall -c -o lexer.o lexer.c
	
frontend.o: 
	gcc -Wall -c -o frontend.o frontend.c
	
lexer.c: 
	flex -t lexer.l > lexer.c
	
clean: 
	rm *.o pa1
	
	
	