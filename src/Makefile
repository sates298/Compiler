BFLAGS=-d -Wconflicts-sr
CFLAGS=-Wall --pedantic -std=c++17
# CFLAGS = -Weverything -std=c++17

CXX=g++ $(CFLAGS)
BISON=bison $(BFLAGS)
FLEX=flex


.PHONY = all clean

all: compiler

flex:
	$(FLEX) -o lexer.cpp lexer.l

bison:
	$(BISON) -o yacc.cpp yacc.y

parser: flex bison ast
	$(CXX) -o yacc.o -c yacc.cpp
	$(CXX) -o lexer.o -c lexer.cpp

ast:
	$(CXX) -o handlers.o -c code/AST/handlers.cpp
	$(CXX) -o variables.o -c code/AST/variables.cpp
	$(CXX) -o blocks.o -c code/AST/blocks.cpp
	$(CXX) -o tree.o -c code/AST/tree.cpp

trans:
	$(CXX) -o support.o -c code/transitional/support.cpp
	$(CXX) -o validator.o -c code/transitional/validator.cpp
	$(CXX) -o transitional.o -c code/transitional/transitional_state.cpp
	$(CXX) -o pseudocode.o -c code/transitional/pseudocode_generator.cpp

assembler:
	$(CXX) -o asm_generator.o -c code/assembler/assembler_generator.cpp
	$(CXX) -o assembler.o -c code/assembler/assembler.cpp

main: trans assembler
	$(CXX) -o global.o -c code/global.cpp
	$(CXX) -o errors.o -c code/errors.cpp
	$(CXX) -o main.o -c code/main.cpp

compiler: parser main
	$(CXX) -o compiler lexer.o yacc.o handlers.o assembler.o asm_generator.o variables.o \
		 blocks.o tree.o validator.o transitional.o support.o pseudocode.o global.o errors.o main.o 

test:
	./compiler < ../tests/test.imp > ../tests/test.mr

clean:
	$(RM) *.o

cleanall: clean
	$(RM) yacc.cpp yacc.hpp lexer.cpp