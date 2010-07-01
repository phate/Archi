CFLAGS += -g -Wall -std=gnu99
LDFLAGS += -lfl -ltalloc
LEX=flex
YACC=bison
YFLAGS += -d

all: parser.o scanner.o cgen/cgen.o cgen/regsect.o ehandling.o tc/typecheck.o tc/trim.o tc/instrsect.o tc/regsect.o symtab.o main.o ast/node.o ast/attributes.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

scanner.c: scanner.l
	$(LEX) -o $@ $^

parser.c: parser.y
	$(YACC) $(YFLAGS) -o $@ $^

clean:
	$(RM)  cgen/*.o tc/*.o ast/*.o *.o scanner.c parser.c parser.h a.out
