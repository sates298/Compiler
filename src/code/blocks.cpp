#include "../headers/blocks.hpp"

// ------------------------------------CODE-BLOCK-------------------------------------

CodeBlock::CodeBlock(BlockType blockType): blockType(blockType){
    this -> parent = nullptr;
}
std::shared_ptr<CodeBlock> CodeBlock::getParent(){
    return this -> parent;
}
void CodeBlock::setParent(std::shared_ptr<CodeBlock> parent){
    this->parent = parent;
}
BlockType CodeBlock::getBlockType(){
    return this -> blockType;
}
std::map<std::string, std::shared_ptr<Variable>> CodeBlock::getLocalVariables(){
    std::shared_ptr<CodeBlock> curr(this);
    std::map<std::string, std::shared_ptr<Variable>> localVariables;
    while(curr.get()->parent != nullptr){
        if(curr.get()->blockType == CMD){
            //todo zrobic virtual == 0 jako gettery
            std::shared_ptr<Command> cmd = std::dynamic_pointer_cast<Command>(curr);
            if(cmd.get() -> getType() == CFOR || cmd.get() -> getType() == CFORDOWN){
                localVariables[cmd.get()->getIterator().get()->getName()] = cmd.get()->getIterator();
            }
        }
    }
    return localVariables;
}
// ------------------------------------COMMAND-------------------------------------
Command::Command(std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type): CodeBlock(CMD), nested(nested), type(type){
    for(auto block:nested){
        block.get()->setParent(std::make_shared<CodeBlock>(this));
    }
}

Command::Command(std::shared_ptr<CodeBlock> nested, CommandType type): CodeBlock(CMD), type(type){
    this->nested.emplace_back(nested);
    nested.get()->setParent(std::make_shared<CodeBlock>(this));
}

void Command::setForLoop(std::shared_ptr<Variable> iterator){
    this->iterator = iterator;
}

std::vector<std::shared_ptr<CodeBlock>> Command::getNested(){
    return this -> nested;
}

std::shared_ptr<Variable> Command::getIterator(){
    return this->iterator;
}

CommandType Command::getType(){
    return this -> type;
}
// ------------------------------------EXPRESSION-------------------------------------

Expression::Expression(value leftValue, ExpressionType expr, value rightValue):CodeBlock(EXPR), leftValue(leftValue), expr(expr), rightValue(rightValue){
    checkResult();
}

void Expression::checkResult(){
    long long a,b;
    if(this->leftValue.var.get() == nullptr){
        a = this->leftValue.val;
    }else if(this->leftValue.var.get()->isConstant() && 
            this->leftValue.var.get()->isDeclared()){
        a = this->leftValue.var.get()->getValue();
    }else{
        return;
    }

    if(this->rightValue.var.get() == nullptr){
        b = this->rightValue.val;
    }else if(this->rightValue.var.get()->isConstant() && 
            this->rightValue.var.get()->isDeclared()){
        b = this->rightValue.var.get()->getValue();
    }else{
        return;
    }

    this->computeResult(a,b);
}

void Expression::computeResult(long long a, long long b){
    switch (this->expr)
    {
    case EPLUS:
        //todo sprawdzenie zakresu
        this->result = a + b;
        break;
    case EMINUS:
        //todo sprawdzenie zakresu
        this->result = a - b;
        break;
    case ETIMES:
        //todo sprawdzenie zakresu
        this->result = a*b;
        break;
    case EDIV:
        if(b == 0){
            this->result=0;
        }else if(a*b <0 && a%b != 0){
            this->result = (a / b) - 1;
        }else{
            this->result = a / b;
        }
        break;
    case EMOD:
        if(b == 0){
            this->result=0;
        }else if(a*b <0 && a%b != 0){
            this->result = (a % b) + b;
        }else{
            this->result = a % b;
        }
        break;
    default:
        this->resultExists = false;
        return;
    }
    this->resultExists = true;
}

bool Expression::isResultExist(){
    return this->resultExists;
}
long long Expression::getResult(){
    return this -> result;
}
value Expression::getLeft(){
    return this -> leftValue;
}
ExpressionType Expression::getExpr(){
    return this->expr;
}
value Expression::getRight(){
    return this->rightValue;
}

// ------------------------------------CONDITION-------------------------------------

Condition::Condition(value leftValue, ConditionType cond, value rightValue):CodeBlock(COND), leftValue(leftValue), cond(cond), rightValue(rightValue){
    checkResult();
}

void Condition::checkResult(){
    long long a,b;
    if(this->leftValue.var.get() == nullptr){
        a = this->leftValue.val;
    }else if(this->leftValue.var.get()->isConstant() && 
            this->leftValue.var.get()->isDeclared()){
        a = this->leftValue.var.get()->getValue();
    }else{
        return;
    }

    if(this->rightValue.var.get() == nullptr){
        b = this->rightValue.val;
    }else if(this->rightValue.var.get()->isConstant() && 
            this->rightValue.var.get()->isDeclared()){
        b = this->rightValue.var.get()->getValue();
    }else{
        return;
    }

    this->computeResult(a,b);
}

void Condition::computeResult(long long a, long long b){
    switch (this->cond)
    {
    case CEQ:
        this->result = a == b;
        break;
    case CNEQ:
        this->result = a != b;
        break;
    case CLE:
        this->result = a < b;
        break;
    case CGE:
        this->result = a > b;
        break;
    case CLEQ:
        this->result = a <= b;
        break;
    case CGEQ:
        this->result = a >= b;
        break;
    default:
        this->resultExists = false;
        return;
    }
    this->resultExists = true;
}


bool Condition::isResultExist(){
    return this->resultExists;
}
bool Condition::getResult(){
    return this->result;
}
value Condition::getLeft(){
    return this -> leftValue;
}
ConditionType Condition::getCond(){
    return this->cond;
}
value Condition::getRight(){
    return this->rightValue;
}
