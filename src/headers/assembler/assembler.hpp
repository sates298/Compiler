#ifndef ASM_HPP
#define ASM_HPP
#include "../typedefs.hpp"

#include<vector>
#include<memory>


enum Instruction{
    GET, PUT, LOAD, STORE, LOADI,
     STOREI, ADD, SUB, SHIFT, INC,
     DEC, JUMP, JPOS, JZERO, JNEG, HALT
};

std::string instructionToString(Instruction instr);

class Asm{
    private:
        Instruction instr;
        bool declared = false;
        uint64 addr;
    public:
        Asm(Instruction inst, uint64 addr);
        ~Asm(){}
        Instruction getInstr();
        uint64 getAddr();

        std::string toString();
};

typedef std::vector<std::shared_ptr<Asm>> asmVec;

extern uint64 addr;
extern asmVec finalCode;

void generateRealRegisters();
void generateFromPseudoAsm();

#endif