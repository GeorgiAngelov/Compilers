PROGRAM = pa2
CFILES = frontend.c
HEADERS = 
YFILE = parser.y
LFILE = lexer.l

CC = gcc
CFLAGS = -Wall
LEX = flex
YACC = bison

# Autogenerate headers as well as *.c files.
YFLAGS = --defines=$(YFILE:.y=.h) -o y.tab.c
LFLAGS = --header-file=$(LFILE:.l=.h)

$(PROGRAM): $(YFILE:.y=.o) $(LFILE:.l=.o) $(CFILES:.c=.o) $(HEADERS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	-rm $(YFILE:.y=.o) $(YFILE:.y=.c) $(YFILE:.y=.h)
	-rm $(LFILE:.l=.o) $(LFILE:.l=.c) $(LFILE:.l=.h)
	-rm $(CFILES:.c=.o)
	-rm $(PROGRAM)
