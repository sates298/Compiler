#include "errors.hpp"

extern char* yytext;
extern int yylval;
extern int yylineno;

extern "C" int yywrap();
int yylex();
int yyparse();
void yyerror(const char* s);