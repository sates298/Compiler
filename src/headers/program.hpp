#ifndef PROG_HPP
#define PROG_HPP

#include "../yacc.hpp"
#include "global.hpp"
#include "./transitional/transitional_state.hpp"

void run(){
    yyparse();
    valid();
    generatePseudoCode();
}
#endif