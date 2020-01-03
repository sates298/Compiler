BFLAGS=-d -Wconflicts-sr
CFLAGS=-Wall --pedantic -std=c++17


CXX=g++ $(CFLAGS)
BISON=bison $(BFLAGS)
FLEX=flex


.PHONY = all clean

all: compiler

flex:
	$(FLEX) -o lexer.cpp lexer.l

bison:
	$(BISON) -o yacc.cpp yacc.y

parser: flex bison
	$(CXX) -o yacc.o -c yacc.cpp
	$(CXX) -o lexer.o -c lexer.cpp

handlers: parser
	$(CXX) -o handlers.o -c code/handlers.cpp

classes: handlers
	$(CXX) -o assembler.o -c code/assembler.cpp
	$(CXX) -o variables.o -c code/variables.cpp
	$(CXX) -o blocks.o -c code/blocks.cpp
	$(CXX) -o tree.o -c code/tree.cpp

main: classes
	$(CXX) -o errors.o -c code/errors.cpp
	$(CXX) -o main.o -c code/main.cpp

compiler: parser main
	$(CXX) -o compiler lexer.o yacc.o assembler.o variables.o blocks.o tree.o errors.o main.o 

test:
	./compiler < ../tests/test.imp > ../tests/test.mr

clean:
	$(RM) *.o

cleanall: clean
	$(RM) yacc.cpp yacc.hpp lexer.cpp