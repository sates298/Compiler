#include "../../headers/assembler/assembler.hpp"

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

std::string instructionToString(Instruction instr){
    switch (instr)
    {
    case GET:
        return "GET";
    case PUT:
        return "PUT";
    case LOAD:
        return "LOAD";
    case STORE:
        return "STORE";
    case LOADI:
        return "LOADI";
    case STOREI:
        return "STOREI";
    case ADD:
        return "ADD";
    case SUB:
        return "SUB";
    case SHIFT:
        return "SHIFT";
    case INC:
        return "INC";
    case DEC:
        return "DEC";
    case JUMP:
        return "JUMP";
    case JPOS:
        return "JPOS";
    case JZERO:
        return "JZERO";
    case JNEG:
        return "JNEG";
    case HALT:
        return "HALT";
    default:
        return "null";
    }
}

std::string Asm::toString(){
    std::string result = instructionToString(this->instr);
    if(this->declared){
        result += " " + std::to_string(this->addr);
    }
    return result;
}