#include "../../headers/transitional/transitional_state.hpp"

PseudoAsm::PseudoAsm(uint64 k, Instruction i, std::string arg) : index(k), instr(i), argument(arg){
}
void PseudoAsm::shiftCode(uint64 from, int64 how, pseudoQue code){
    for(int i=from; i<code.size(); i++){
        code[i]->shiftIndex(how);
    }
}
uint64 PseudoAsm::getIndex(){
    return this->index;
}
void PseudoAsm::shiftIndex(int64 s){
    this->index += s;
}
void PseudoAsm::setJumpReference(std::shared_ptr<PseudoAsm> reference){
    this->jumpReference = reference;
    reference->setJumped(true);
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

void PseudoAsm::setJumped(bool b){
    this->jumped = b;
}

bool PseudoAsm::isJumped(){
    return this->jumped;
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

std::string PseudoRegister::toString(){
    std::string result = "{" + this->name;

    if(this->isNumber){
        result += ", number=true";
        if(this->isOffset){
            result += ", offset=true, offset value="+std::to_string(this->offsetVal);
        }
    }else if(this->var != nullptr){
        if(var->isArray()){
            result += ", var="+((ArrayVariable *)this->var)->toString();
        }else{
            result += ", var="+this->var->toString();
        }
    }else{
        if(this->isIterator){
            result += ", iterator=true";
        }
    }
    result += ", addr=" + std::to_string(this->index);
    result += "}";
    return result;
}