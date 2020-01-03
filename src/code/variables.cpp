#include "../headers/variables.hpp"

Variable::Variable(std::string name) : name(name), type(VAL) {}

void Variable::setValue(long long value){
    this -> value = value;
}
void Variable::setConstant(bool constant){
    this -> constant = constant;
}
void Variable::setDeclared(bool declared){
    this -> declared = declared;
}

std::string Variable::getName(){
    return this -> name;
}
long long Variable::getValue(){
    return this -> value;
}
bool Variable::isConstant(){
    return this -> constant;
}
bool Variable::isDeclared(){
    return this -> declared;
}
bool Variable::isArray(){
    return this -> type == ARR;
}

ArrayVariable::ArrayVariable(std::string name ,long long a, long long b) : Variable(name) {
    if (a > b){
        throw -1;
    }else{
        this->type = ARR;
        this->firstIdx = a;
        this->lastIdx = b;
    }
}

void ArrayVariable::setElement(long long idx, std::shared_ptr<Variable> element){
    if (idx < this->firstIdx || idx > this->lastIdx){
        throw -1;
    }else{
        this->elements[idx] = element;
    }
}

std::shared_ptr<Variable> ArrayVariable::getElement(long long idx){
    if (idx < this->firstIdx || idx > this->lastIdx){
        throw -1;
    }else{
        return this->elements[idx];
    }
}
