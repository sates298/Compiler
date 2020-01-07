#include "../../headers/transitional/validator.hpp"

void valid(){
    for(auto& r: tree.getRoots()){
        valid(r.get());
    }
}

void valid(CodeBlock *block){
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

ValidVar validDeclaredCall(Call cal, CodeBlock *parent){
    std::string name = cal.name;
    bool arrayCall = cal.isFirstIndex || cal.secondIdx != "";
    auto declared = tree.getVariables();
    auto iterators = parent != nullptr ? parent->getLocalVariables() : std::map<std::string,std::shared_ptr<Variable>>();
    
    Variable *found;
    ValidVar r = ValidVar();
    if( declared.find(name) == declared.end()){
        if(iterators.find(name) == iterators.end()){
            error( name + " is undefined", cal.line , true);
            return r;
        }else{
            found = iterators[name].get();
            r.isIterator = true;
        }
    }else{
        found = declared[name].get();
        r.isIterator = false;
    }
    if(arrayCall && !found->isArray()){
        error("Wrong call to variable " + found->getName(), "You need to delete '" + cyan + "(?)" + green + "' block",cal.line, true);
    }else if(!arrayCall && found->isArray()){
        error("Wrong call to array " + found->getName(),
         "You need to add call to exact element like '" + cyan + found->getName() + "(x)" + green +"', where x is valid index.", cal.line, true);
    }

    if(!arrayCall){
        r.isConstant = found->isConstant();
        r.isDeclared = found->isDeclared();
        return r;
    }
    
    if(!cal.isFirstIndex){
        Call indexCall = Call();
        indexCall.line = cal.line;
        indexCall.name = cal.secondIdx;
        indexCall.isFirstIndex = false;
        auto isIdx = validDeclaredCall(indexCall, parent);
        
        if(isIdx.isDeclared && !isIdx.isConstant){
            warning("Call to array " + cal.name + " by not constant index", cal.line);
        }else if(!isIdx.isDeclared){
            error("Call to array " + cal.name + " by undeclared variable", cal.line);
        }else{
            auto i = tree.getVariables()[cal.secondIdx]->getValue();
            auto v = (ArrayVariable *)(tree.getVariables()[cal.name].get());
            try{
                auto el = v->getElement(i);
                if(el != nullptr){
                    r.isConstant = el->isConstant();
                    r.isDeclared = el->isDeclared();
                }else{
                    r.isConstant = false;
                    r.isDeclared = false;
                }
            }catch(int i){
                warning("Call to element out of the array", cal.line);
            }
        }
        r.isIterator = false;
        return r;
    }

    int64 idx = cal.firstIdx;

    ArrayVariable *f = (ArrayVariable *)found;
    try{
        auto el = f->getElement(idx);
        if(el != nullptr){
            r.isConstant = el->isConstant();
            r.isDeclared = el->isDeclared();
            return r;
        }
    }catch(int i){
        warning("Call to element out of the array", cal.line);
    }
}

ValidVar validVal(Value *val, CodeBlock *parent){
    
    if(val->cal != nullptr){
        auto declared = validDeclaredCall(*(val->cal.get()), parent);
        if(!declared.isDeclared){
            error("Wanted value is undeclared",val->cal->line, true);
        }
        return declared;
    }else{
        ValidVar r = ValidVar();
        r.isIterator = false;
        r.isConstant = true;
        r.isDeclared = true;
        return r;
    }
}

void valid(Command *cmd){
    if(cmd->getType() == CFOR || cmd->getType() == CFORDOWN){
        ForLoop *fl = (ForLoop *)(cmd);
        validFor(fl);
        for(auto& n: cmd->getNested()){
            valid(n.get());
        }
    }else if(cmd->getType() == CWRITE){
        validWrite(cmd);
    }else if(cmd->getType() == CREAD){
        validRead(cmd);
    }else if(cmd->getType() == CASSIGN){
        validAssign(cmd);
    }else if(cmd->getType() == CIF || cmd->getType() == CIFELSE){
        validIf(cmd);
        for(auto& n: cmd->getNested()){
            valid(n.get());
        }
    }else if(cmd->getType() == CWHILE) {
        validWhile(cmd);
        for(auto& n: cmd->getNested()){
            valid(n.get());
        }
    }else if(cmd->getType() == CDOWHILE){
        for(auto& n: cmd->getNested()){
            valid(n.get());
        }
        validWhile(cmd);
    }
    
}

void validWrite(Command *cmd){
    auto c = cmd->getValue().cal;
    if(c != nullptr){
        auto r = validDeclaredCall(*(c.get()), cmd);
        if(!r.isDeclared){
            error("Attempting to WRITE undeclared value",cmd->getLastLine(), false);
        }
    }
}

void validRead(Command *cmd){
    auto v = validDeclaredCall(*(cmd->getCalls()[0].get()), cmd);
    if(v.isIterator){
        error("Attempting to change iterator",cmd->getLastLine(), true);
    }
    auto name = cmd->getCalls()[0]->name;
    tree.getVariables()[name]->setDeclared(true);
    tree.getVariables()[name]->setConstant(false);
}

void validAssign(Command *cmd){
    auto v = validDeclaredCall(*(cmd->getCalls()[0].get()), cmd);
    if(v.isIterator){
        error("Attempting to change iterator",cmd->getLastLine(), true);
    }
    auto name = cmd->getCalls()[0]->name;
    

    auto exp = std::dynamic_pointer_cast<Expression>(cmd->getNested()[0]);
    auto get = validExp(exp.get());
    auto var = tree.getVariables()[name];
    
    if(get.isValue){
        if(!var->isDeclared()){
            var->setDeclared(true);
            var->setConstant(true);
            var->setValue(get.value);
        }else{
            var->setConstant(false);
        }
    }else{
        var->setConstant(false);
    }
}

void validIf(Command *cmd){
    auto cond = validCond((Condition *)(cmd->getNested()[0].get()));
    if(cond.isConstVal && !cond.value && cmd->getType() == CIF){
        warning("Never reached interior of IF block",cmd->getLastLine());
    }else if(cond.isConstVal && !cond.value && cmd->getType() == CIFELSE){
        warning("Never reached first interior of IF ELSE block",cmd->getLastLine());
    }else if(cond.isConstVal && cond.value && cmd->getType() == CIFELSE){
        warning("Never reached second interior of IF ELSE block",cmd->getLastLine());
    }else if(cond.isConstVal && cond.value && cmd->getType() == CIF){
        warning("Always reached interior of IF block",cmd->getLastLine());
    }
}

void validWhile(Command *cmd){
    auto cond = validCond((Condition *)(cmd->getNested()[0].get()));
    if(cond.isConstVal && !cond.value && cmd->getType() == CWHILE){
        warning("Never reached interior of WHILE block", cmd->getLastLine());
    }else if(cond.isConstVal && cond.value){
        warning("Probably infinite loop", cmd->getLastLine());
    }
}

void validFor(ForLoop *forl){
    auto name = forl->getIterator()->getName();
    auto globals = tree.getVariables();
    auto p = forl->getParent().lock();
    forl->setParent(p);
    if(p != nullptr){
        auto vars = p->getLocalVariables();
        if(vars.find(name) != vars.end() || globals.find(name) != globals.end()){
            error ("variable " + name + " is already initialized", true);
        }
    }

    // p = forl->getParent().lock();
    validVal(&(forl->getFrom()), p.get());
    validVal(&(forl->getTo()), p.get());
    forl->setParent(p);
    auto from = forl->getFrom();
    auto to = forl->getTo();

    if(forl->getType() == CFOR){
        if(from.cal == nullptr && to.cal == nullptr){
            if(from.val > to.val){
                warning("Never reached FOR block",forl->getLastLine());
            }
        }else{
            if(optimization){
                //todo sprawdzanie po zmiennych
            }
        }
    }else{
        if(from.cal == nullptr && to.cal == nullptr){
            if(from.val < to.val){
                warning("Never reached FOR block",forl->getLastLine());
            }
        }else{
            if(optimization){
                //todo sprawdzanie po zmiennych
            }
        }
    }
}
ValidExp validExp(Expression *expr){
    auto left = validVal(&(expr->getLeft()), expr);
    auto right = validVal(&(expr->getRight()), expr);

    ValidExp exp = ValidExp();
    exp.isValue = false;
    if(expr->getExpr() == ENULL){
        if(!isInLoop(expr)){
            if(left.isDeclared && left.isConstant){
                exp.isValue = true;
                exp.value = getValueFromConstant(expr->getLeft());
                return exp;
            }
        }
    }
    if(optimization){
        //todo liczenie wartości
    }


    return exp;
}

ValidCond validCond(Condition *cond){
    auto left = validVal(&(cond->getLeft()), cond);
    auto right = validVal(&(cond->getRight()), cond);

    ValidCond result = ValidCond();
    result.isConstVal = false;
    if(optimization){
        if(!isInLoop(cond)){
            if(cond->getLeft().cal == nullptr){
                bool value;
                int64 l = cond->getLeft().val;
                int64 r;
                if(cond->getRight().cal == nullptr){
                    r = cond->getRight().val;
                    value = getCond(l, r, cond->getCond());
                }else{
                    if(right.isConstant){
                        //todo if right.cal is constant;
                    }
                }

                result.isConstVal = true;
                result.value = value;
            }else{
                if(left.isConstant){
                    int64 l = 1; //todo get left.value
                    if(cond->getRight().cal == nullptr){
                        int64 r = cond->getRight().val;
                        auto value = getCond(l, r, cond->getCond());
                        result.isConstVal = true;
                        result.value = value;
                    }else{
                        if(right.isConstant){
                            //todo if right.cal is constant;
                        }
                        
                    }
                }
                //todo if left.cal is constant
            }
        }
    }

    return result;
}