#ifndef ASM_HPP
#define ASM_HPP

enum Instruction{
    GET, PUT, LOAD, STORE, LOADI,
     STOREI, ADD, SUB, SHIFT, INC,
     DEC, JUMP, JPOS, JZERO, JNEG, HALT
};

class Asm{
    private:
        Instruction instr;
        bool declared = false;
        unsigned long long addr;
    public:
        Asm(Instruction inst, unsigned long long addr);
        ~Asm(){}
        Instruction getInstr();
        unsigned long long getAddr();
};

#endif