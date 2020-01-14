#ifndef ASM_GEN_HPP
#define ASM_GEN_HPP

#include "assembler.hpp"
#include "../transitional/transitional_state.hpp"
#include<cstdlib>
#include<tuple>

#define _PUSH_ASM(a) { finalCode.emplace_back(a); addr++;}

#define _PUSH_INS(instr, reg) {\
    auto assm = std::make_shared<Asm>(instr, reg);\
    _PUSH_ASM(assm)\
}

#define _PUSH_PSEUDO_INS(instr, reg){\
    if(std::string(reg) != "null") {\
        auto address = registers[reg];\
        _PUSH_INS(instr, address->index)\
    }else{\
        _PUSH_INS(instr, 0)\
    }\
}
extern uint64 addr;
extern uint64 regIdx;

void generateNumber(std::string num);
void generateConstants();
extern void smartGeneratingConstants();
void generateAsm(PseudoAsm *p);

void generateStore(PseudoAsm *p);
void generateLoad(PseudoAsm *p);
void generateStorei(PseudoAsm *p);
void generateLoadi(PseudoAsm *p);
void generateJump(PseudoAsm *p);
void generateJzero(PseudoAsm *p);
void generateJpos(PseudoAsm *p);
void generateJneg(PseudoAsm *p);
void generateShift(PseudoAsm *p);
void generateAdd(PseudoAsm *p);
void generateSub(PseudoAsm *p);
#endif