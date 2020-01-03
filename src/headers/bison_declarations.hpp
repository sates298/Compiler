#ifndef BISON_D__HPP
#define BISON_D_HPP
#include "errors.hpp"
#include "handlers.hpp"

extern int yylineno;
int yyparse();
int yylex();
void yyerror(const char *);
#endif