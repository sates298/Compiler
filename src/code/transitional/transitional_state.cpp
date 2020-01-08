#include "../../headers/transitional/transitional_state.hpp"

PseudoAsm::PseudoAsm(uint64 k, Instruction i, std::string arg) : index(k), instr(i), argument(arg){
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
}
void PseudoAsm::setJumpReference(std::shared_ptr<PseudoAsm> reference){
    this->jumpReference = reference;
}

std::shared_ptr<PseudoAsm> PseudoAsm::getJumpReference(){
    return this->jumpReference;
}
Instruction PseudoAsm::getInstr(){
    return this->instr;
}
std::string PseudoAsm::getArgument(){
    return this->argument;
}

std::string PseudoAsm::toString(){
    std::string result = std::to_string(this->index) + ". ";
    std::string instr = instructionToString(this->instr);
    std::string call = this->argument;

    result += instr + " ";
    if(call != "null"){
        result += call;
    }else if(this->jumpReference != nullptr){
        result += std::to_string(this->jumpReference->index);
    }
    return result;
}