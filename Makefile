CC = gcc
CFLAGS = -Wall -g
LEX = flex
YACC = bison -d -v
LIBS = -lfl

SRCS = ast.c symbol_table.c semantic.c main.c ir.c
OBJS = $(SRCS:.c=.o)

all: compiler

compiler: parser.tab.c lex.yy.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ parser.tab.c lex.yy.c $(OBJS) $(LIBS)

parser.tab.c parser.tab.h: parser.y
	$(YACC) -o parser.tab.c parser.y

lex.yy.c: lexer.l parser.tab.h
	$(LEX) -o lex.yy.c lexer.l

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f compiler parser.tab.c parser.tab.h parser.output lex.yy.c *.o output.cs output.ir

run: compiler
	./compiler input.txt

.PHONY: all clean run


#   cd /mnt/f/compiler_lab/compiler3


#    ./compiler_terminal.sh