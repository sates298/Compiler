#include "../headers/blocks.hpp"

// ------------------------------------CODE-BLOCK-------------------------------------

CodeBlock::CodeBlock(BlockType blockType,int64 startLine): blockType(blockType), startLine(startLine){}

std::weak_ptr<CodeBlock> CodeBlock::getParent(){
    return this -> parent;
}
void CodeBlock::setParent(std::shared_ptr<CodeBlock> parent){
    this->parent = parent;
}
BlockType CodeBlock::getBlockType(){
    return this -> blockType;
}
std::map<std::string, std::shared_ptr<Variable>> CodeBlock::getLocalVariables(){
    CodeBlock *curr = this;
    std::map<std::string, std::shared_ptr<Variable>> localVariables;
    while(curr != nullptr){
        if(curr->blockType == CMD && (((Command *)curr)->getType() == CFOR 
                                || ((Command*)curr)->getType() == CFORDOWN)){
            ForLoop *cmd = (ForLoop *)(curr);
            localVariables[cmd->getIterator()->getName()] = cmd->getIterator();
        }
        
        auto p = curr->getParent().lock();
        curr->setParent(p);
        curr = p.get();
    }

    return localVariables;
}
std::vector<std::shared_ptr<Call>> &CodeBlock::getCalls(){
    return this->calls;
}

int64 CodeBlock::getStartLine(){
    return this->startLine;
}

// ------------------------------------COMMAND-------------------------------------
Command::Command(CommandType type,int64 startLine): CodeBlock(CMD,startLine), type(type){}

Command::Command(std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type,int64 startLine): Command(type, startLine){
    this->nested = std::vector<std::shared_ptr<CodeBlock>>(nested);
}

Command::Command(std::shared_ptr<CodeBlock> nested, CommandType type,int64 startLine): Command(type, startLine){
    this->nested.push_back(nested);
}

void Command::appendBlocks(std::vector<std::shared_ptr<CodeBlock>> blocks){
    for(auto& b:blocks){
        this->nested.push_back(b);
    }
}

void Command::setParentForAll(){
    auto p = shared_from_this();
    for(auto& b:this->nested){
        b->setParent(p);
        if(b->getBlockType() == CMD){
            auto tmp = std::dynamic_pointer_cast<Command>(b);
            tmp->setParentForAll();
        }
    }
}

std::vector<std::shared_ptr<CodeBlock>> &Command::getNested(){
    return this -> nested;
}

CommandType Command::getType(){
    return this -> type;
}
//CIFELSE
int64 Command::getFirstElseIndex(){
    return this->firstElseIndex;
}

void Command::setFirstElseIndex(int64 idx){
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
                     std::shared_ptr<CodeBlock> nested, CommandType type, int64 startLine):
                        Command(nested, type, startLine), from(from), to(to) {
    this->iterator = std::make_shared<Variable>(iterator);
}
ForLoop::ForLoop(std::string iterator, Value from, Value to,
                 std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type, int64 startLine):
                    Command(nested, type, startLine), from(from), to(to){
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

Expression::Expression(Value leftValue, ExpressionType expr, Value rightValue, int64 startLine)
    :CodeBlock(EXPR, startLine), leftValue(leftValue), expr(expr), rightValue(rightValue){}

bool Expression::isResultExist(){
    return this->resultExists;
}
int64 Expression::getResult(){
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

Condition::Condition(Value leftValue, ConditionType cond, Value rightValue, int64 startLine)
    :CodeBlock(COND,startLine), leftValue(leftValue), cond(cond), rightValue(rightValue){}

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
            calls += c->toString();
        }
        calls += "]";

        std::string nested = "[";
        for(auto n: this->nested){
            nested += "\t" + n->toString();
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
        calls += c->toString();
    }
    calls += "]";

    std::string nested = "[";
    for(auto n: this->nested){
        nested += n->toString();
    }
    nested += "]";
    std::string it = this->iterator->toString(); 
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