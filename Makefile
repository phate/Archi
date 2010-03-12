CFLAGS += -g -Wall -std=gnu99
LDFLAGS += -lfl
LEX=flex
YACC=bison
YFLAGS += -d

all: cgen.o ehandling.o analyze.o trim.o symtab.o parser.o scanner.o main.o nodes.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

scanner.c: scanner.l
	$(LEX) -o $@ $^

parser.c: parser.y
	$(YACC) $(YFLAGS) -o $@ $^

clean:
	$(RM) *.o scanner.c parser.c parser.h a.out
