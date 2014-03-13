PROGRAM = pa2
CFILES = new_driver.c
HEADERS = 
YFILE = microextendedcount_and_idlist.y
LFILE = microextendedcount_and_idlist.l

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
