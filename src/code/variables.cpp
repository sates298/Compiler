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

std::string Variable::toString(){
    std::string result;
    if(this->type == VAL){
        result += "Variable:{name=" + this->name + "}\n";
    }else{
        ArrayVariable *a = (ArrayVariable *)this;
        auto arr = std::make_shared<ArrayVariable>(*a);
        result = arr.get()->toString(); 
    }
    return result;
}

std::string ArrayVariable::toString(){
    std::string result = "ArrayVariable:{name=" + this->name + ", from=" + std::to_string(this -> firstIdx) + ", to=" + std::to_string(this -> lastIdx);

    result += ", array=[";
    for(const auto& [key, val]:this->elements){
        result += "idx=" + std::to_string(key) + ", val=" + val.get()->toString() + ", ";
    }
    result += "]}\n";
    return result;
}

std::string Call::toString(){
    std::string result = "Call:{name=" + this->name;
    if(this->isFirstIndex){
        result += ", callingIdx=" + std::to_string(this->firstIdx);
    }else if(this->secondIdx != ""){
        result += ", callingIdx=" + this->secondIdx;
    }
    result += "}";
    return result;
}

std::string Value::toString(){
    std::string result = "Value:{val=";
    if(this->cal.get() != nullptr){
        result += this->cal.get()->toString();
    }else{
        result += std::to_string(this->val);
    }
    result += "}";
    return result;
}