#include "../../headers/transitional/support.hpp"

bool getCond(int64 l, int64 r, ConditionType type){
    bool result;
    switch (type)
    {
        case CEQ:
            result = l==r;
            break;
        case CNEQ:
            result = l!=r;
            break;
        case CGE:
            result = l > r;
            break;
        case CLE:
            result = l < r;
            break;
        case CGEQ:
            result = l >= r;
            break;
        case CLEQ:
            result = l <= r;
            break;
        default:
            result = false;
            break;
    }
    return result;
}

int64 getValueFromConstant(Value v){
    auto l = v.cal;
    int64 result;
    if(l != nullptr){
        if(l->isFirstIndex){
            auto arr = ((ArrayVariable *)(tree.getVariables()[l->name].get()));
            result = arr->getElement(l->firstIdx)->getValue();
        }else if(l->secondIdx != ""){
            auto arr = ((ArrayVariable *)(tree.getVariables()[l->name].get()));
            auto idx = tree.getVariables()[l->secondIdx];
            result = arr->getElement(idx->getValue())->getValue(); 
        }else{
            result = tree.getVariables()[l->name]->getValue();
        }
    }else{
        result = v.val;
    }
    return result;
}


bool isInLoop(CodeBlock *c){
    CodeBlock *curr = c;
    while(curr != nullptr){
        if(curr->getBlockType() == CMD){
            Command *cmd = (Command *)curr;
            if(cmd->getType() == CFOR || cmd->getType() == CFORDOWN 
            || cmd->getType() == CWHILE || cmd->getType() == CDOWHILE){
                return true;
            }
        }
        auto p = curr->getParent().lock();
        curr->setParent(p);
        curr = p.get();
    }
    return false;
}

std::string getCallName(Call c){
    if(c.isFirstIndex){
        return c.name + ":" + std::to_string(c.firstIdx);
    }else if(c.secondIdx != ""){
        return c.name + ":" + c.secondIdx;
    }else{
        return c.name;
    }
}

std::string getValueName(Value val){
    if(val.cal != nullptr){
        return getCallName(*(val.cal.get()));
    }else{
        return std::to_string(val.val);
    }
}