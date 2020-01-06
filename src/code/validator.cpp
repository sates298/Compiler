#include "../headers/validator.hpp"

void valid(){
    for(auto& r: tree.getRoots()){
        valid(r.get());
    }
}

void valid(CodeBlock *block){
    for(auto& c:block->getCalls()){
        valid(*(c), block);
    }

    if(block->getBlockType() == EXPR){
        Expression *exp = (Expression *)(block);
        valid(exp);
    }else if(block->getBlockType() == CMD){
        Command *cmd = (Command *)(block);
        valid(cmd);
    }else if(block->getBlockType() == COND){
        Condition *cond = (Condition *)(block);
        valid(cond);
    }
}

void valid(Call cal, CodeBlock *parent){
    std::string name = cal.name;
    bool arrayCall = cal.isFirstIndex || cal.secondIdx != "";
    auto declared = tree.getVariables();
    auto iterators = parent->getLocalVariables();
    
    Variable *found;
    if( declared.find(name) == declared.end()){
        if(iterators.find(name) == iterators.end()){
            error( name + " is undefined", cal.line , true);
        }else{
            found = iterators[name].get();
        }
    }else{
        if(iterators.find(name) != iterators.end()){
            error ("variable " + name + " is already initialized", true);
        }
        found = declared[name].get();
    }
    if(arrayCall && !found->isArray()){
        error("Wrong call to variable " + found->getName(), "You need to delete '" + cyan + "(?)" + green + "' block",cal.line, true);
    }else if(!arrayCall && found->isArray()){
        error("Wrong call to array " + found->getName(),
         "You need to add call to exact element like '" + cyan + found->getName() + "(x)" + green +"', where x is valid index.", cal.line, true);
    }

    if(!arrayCall){
        return;
    }
    
    if(!cal.isFirstIndex){
        Call indexCall = Call();
        indexCall.line = cal.line;
        indexCall.name = cal.secondIdx;
        indexCall.isFirstIndex = false;
        valid(indexCall, parent);
        return;
    }

    
    int64 idx = cal.firstIdx;

    ArrayVariable *f = (ArrayVariable *)found;
    try{
        f->getElement(idx);
    }catch(int i){
        warning("Call to element out of the array", cal.line);
    }
}

void valid(Value *val, CodeBlock *parent){

}


void valid(Command *cmd){
    if(cmd->getType() == CFOR || cmd->getType() == CFORDOWN){
        ForLoop *fl = (ForLoop *)(cmd);
        valid(fl);
    }else{
    }
    for(auto& n: cmd->getNested()){
        valid(n.get());
    }
}
void valid(ForLoop *forl){
    if(forl->getType() == CFOR){
        
    }else{

    }
}
void valid(Expression *expr){

}
void valid(Condition *cond){
    
}