#include "../headers/blocks.hpp"

// ------------------------------------CODE-BLOCK-------------------------------------

CodeBlock::CodeBlock(BlockType blockType): blockType(blockType){
    this->parent = nullptr;
}
CodeBlock *CodeBlock::getParent(){
    return this -> parent;
}
void CodeBlock::setParent(CodeBlock *parent){
    this->parent = parent;
}
BlockType CodeBlock::getBlockType(){
    return this -> blockType;
}
std::map<std::string, std::shared_ptr<Variable>> CodeBlock::getLocalVariables(){
    CodeBlock *curr = this;
    std::map<std::string, std::shared_ptr<Variable>> localVariables;
    while(curr->parent != nullptr){
        if(curr->blockType == CMD && (((Command *)curr)->getType() == CFOR 
                                || ((Command *)curr)->getType() == CFORDOWN)){
            ForLoop *cmd = (ForLoop *)(curr);
            if(cmd -> getType() == CFOR || cmd -> getType() == CFORDOWN){
                localVariables[cmd->getIterator().get()->getName()] = cmd->getIterator();
            }
        }
    }
    return localVariables;
}
std::vector<std::shared_ptr<Call>> &CodeBlock::getCalls(){
    return this->calls;
}

// ------------------------------------COMMAND-------------------------------------
Command::Command(CommandType type): CodeBlock(CMD), type(type){}

Command::Command(std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type): Command(type){
    this->appendBlocks(nested);
}

Command::Command(std::shared_ptr<CodeBlock> nested, CommandType type): Command(type){
    this->appendBlock(nested);
}

void Command::appendBlock(std::shared_ptr<CodeBlock> block){
    this->nested.emplace_back(block);
    block.get()->setParent(this);
}

void Command::appendBlocks(std::vector<std::shared_ptr<CodeBlock>> blocks){
    for(auto b:blocks){
        this->appendBlock(b);
    }
}

std::vector<std::shared_ptr<CodeBlock>> &Command::getNested(){
    return this -> nested;
}

CommandType Command::getType(){
    return this -> type;
}
//CIFELSE
long long Command::getFirstElseIndex(){
    return this->firstElseIndex;
}

void Command::setFirstElseIndex(long long idx){
    this->firstElseIndex = idx;
}
//CWRITE
Value &Command::getValue(){
    return this->writeValue;
}
void Command::setValue(Value val){
    this->writeValue = val;
}
// ------------------------------------FORLOOP-------------------------------------
ForLoop::ForLoop(std::string iterator, Value from, Value to,
                     std::shared_ptr<CodeBlock> nested, CommandType type):
                        Command(nested, type), from(from), to(to) {
    this->iterator = std::make_shared<Variable>(iterator);
}
ForLoop::ForLoop(std::string iterator, Value from, Value to,
                 std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type):
                    Command(nested, type), from(from), to(to){
    this->iterator = std::make_shared<Variable>(iterator);
}

std::shared_ptr<Variable> ForLoop::getIterator(){
    return this->iterator;
}

Value &ForLoop::getFrom(){
    return this->from;
}
Value &ForLoop::getTo(){
    return this->to;
}
// ------------------------------------EXPRESSION-------------------------------------

Expression::Expression(Value leftValue, ExpressionType expr, Value rightValue):CodeBlock(EXPR), leftValue(leftValue), expr(expr), rightValue(rightValue){}

bool Expression::isResultExist(){
    return this->resultExists;
}
long long Expression::getResult(){
    return this -> result;
}
Value &Expression::getLeft(){
    return this -> leftValue;
}
ExpressionType Expression::getExpr(){
    return this->expr;
}
Value &Expression::getRight(){
    return this->rightValue;
}

// ------------------------------------CONDITION-------------------------------------

Condition::Condition(Value leftValue, ConditionType cond, Value rightValue):CodeBlock(COND), leftValue(leftValue), cond(cond), rightValue(rightValue){}

bool Condition::isResultExist(){
    return this->resultExists;
}
bool Condition::getResult(){
    return this->result;
}
Value &Condition::getLeft(){
    return this -> leftValue;
}
ConditionType Condition::getCond(){
    return this->cond;
}
Value &Condition::getRight(){
    return this->rightValue;
}

// ------------------------------------TOSTRING-------------------------------------

std::string CodeBlock::toString(){
    std::string result = "CodeBlock:{block=" + std::to_string(this->blockType) + "}\n";
    if(this->blockType == CMD){
        Command *c = (Command *)this;
        result = c->toString(); 
    }else if(this->blockType == EXPR){
        Expression *e = (Expression *)this;
        result = e->toString(); 
    }else{
        Condition *c = (Condition *)this;
        result = c->toString(); 
    }
    return result;
}

std::string Command::toString(){
    std::string result = "cmd";
    
    if(this->type == CFOR || this->type == CFORDOWN){
        ForLoop *f = (ForLoop *)this;
        result = f->toString();
    }else{
        result = "Command:{";
        std::string type = std::to_string(this->type);
        std::string calls = "[";
        for(auto c: this->calls){
            calls += c.get()->toString();
        }
        calls += "]";

        std::string nested = "[";
        for(auto n: this->nested){
            nested += "\t" + n.get()->toString();
        }
        nested += "]";
        result += "type=" + type;
        if(this->type == CIFELSE){
            result += ", first else=" + std::to_string(this->firstElseIndex);
            result += ", nested=" + nested;
            result += ", calls="+calls+"}\n";
        }else if(this->type == CWRITE){
            result += ", writeValue=" + this->writeValue.toString(); 
        }else{
            result += ", nested=" + nested;
            result += ", calls="+calls+"}\n";
        }
    }

    return result;
}

std::string ForLoop::toString(){
    std::string result = "ForLoop:{";
    std::string type = std::to_string(this->type);
    std::string calls = "[";
    for(auto c: this->calls){
        calls += c.get()->toString();
    }
    calls += "]";

    std::string nested = "[";
    for(auto n: this->nested){
        nested += n.get()->toString();
    }
    nested += "]";
    std::string it = this->iterator.get()->toString(); 
    result += "type=" + type + ", iterator=" + it  + ", from=" + this->from.toString() + ", to=" + this-> to.toString() + ", nested=" + nested + ", calls=" + calls + "}\n";
    return result;
}

std::string Expression::toString(){
    std::string result = "Expression:{";
    std::string expr = std::to_string(this->expr);
    result += "left="+this->leftValue.toString()+", expr="+ expr + ", right=" + this->rightValue.toString() + "}\n";
    return result;
}

std::string Condition::toString(){
    std::string result = "Condition:{";
    std::string cond = std::to_string(this->cond);
    result += "left="+this->leftValue.toString()+", cond="+ cond + ", right=" + this->rightValue.toString() + "}\n";
    return result;
}