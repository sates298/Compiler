#include "../headers/main.hpp"

int yywrap(){
    return 1;
}

void yyerror(const char* s){
}

int main(int argc, const char* argv[]){
    yyparse();
    return 0;
}