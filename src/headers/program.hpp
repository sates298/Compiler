#ifndef PROG_HPP
#define PROG_HPP

#include "../yacc.hpp"
#include "./transitional/transitional_state.hpp"
#include "./assembler/assembler.hpp"

void run(){
    yyparse();
    valid();
    if(errorAttempts) return;
    generatePseudoRegisters();
    generatePseudoCode();
    // if(optimization){
        optimizeCode();
    // }
    generateRealRegisters();
    generateFromPseudoAsm();
}
#endif