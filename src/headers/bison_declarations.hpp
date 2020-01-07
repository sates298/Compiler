#ifndef BISON_D__HPP
#define BISON_D_HPP
#include "AST/handlers.hpp"

int yyparse();
int yylex();
void yyerror(const char *);
#endif