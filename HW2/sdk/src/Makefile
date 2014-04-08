PROGRAM = pa1
CFILES = frontend.c
LFILE = lexer.l

CC = gcc
CFLAGS = -Wall
LEX = flex

$(PROGRAM): $(LFILE:.l=.o) $(CFILES:.c=.o)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	-rm $(LFILE:.l=.o) $(LFILE:.l=.c)
	-rm $(CFILES:.c=.o)
	-rm $(PROGRAM)
