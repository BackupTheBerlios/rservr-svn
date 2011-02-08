.PHONY: clean

parser: parser.tab.o lexer.o
	g++ parser.tab.o lexer.o -o parser -lhpaux

parser.tab.h parser.tab.c: parser.y
	bison -Wall -b parser -y -d parser.y

lexer.c: lexer.l
	flex -o lexer.c lexer.l

parser.tab.o: parser.tab.h parser.tab.c
	g++ -g -c -Wall parser.tab.c

lexer.o: parser.tab.h lexer.c
	gcc -g -c -Wall lexer.c

clean:
	rm -fv *~ parser parser.tab.h parser.tab.c lexer.c parser.tab.o lexer.o
