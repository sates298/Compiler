#include "../headers/assembler.hpp"

Asm::Asm(Instruction instr, uint64 addr) : instr(instr){
    switch (instr)
    {
    case PUT:
    case GET:
    case INC:
    case DEC:
    case HALT:
        break;
    default:
        this -> addr = addr;
        this -> declared = true;
        break;
    }
}

Instruction Asm::getInstr(){
    return this -> instr;
}

uint64 Asm::getAddr(){
    return declared ? this -> addr : 0;
}