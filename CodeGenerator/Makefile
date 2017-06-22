TARGET = parser
OBJECT = lex.yy.c y.tab.c y.tab.h
CC = gcc 
LEX = lex
FLAGS = -lfl -ly -lm
YACC = yacc -d -v

all: symTable.c semantic.c lex.yy.c y.tab.c statistic.c codeGen.c 
	${CC} ${FLAGS} $^ -o ${TARGET}

y.tab.c: parser.y
	${YACC} $^	

lex.yy.c: scanner.l
	${LEX} $^

clean:
	rm -rf ${TARGET} ${OBJECT} *.j y.output