#include "../../headers/transitional/transitional_state.hpp"

PseudoAsm::PseudoAsm(uint64 k, Instruction i, std::string arg, bool jump) : index(k), instr(i), argument(arg), inJump(jump){
}
void PseudoAsm::shiftCode(uint64 k, std::vector<std::shared_ptr<PseudoAsm>> code){
    for(auto& a: code){
        a->shiftIndex(k);
    }
}
uint64 PseudoAsm::getIndex(){
    return this->index;
}
void PseudoAsm::shiftIndex(uint64 s){
    this->index += s;
    if(this->instr == JUMP || this->instr == JZERO 
    || this->instr == JPOS || this->instr == JNEG){
        uint64 arg = std::stoull(this->argument);
        arg+=s;
        this->argument = std::to_string(arg); 
    }
}
bool PseudoAsm::isInJump(){
    return this->inJump;
}
Instruction PseudoAsm::getInstr(){
    return this->instr;
}
std::string PseudoAsm::getArgument(){
    return this->argument;
}