#include "../../headers/transitional/validator.hpp"
#include "../../headers/transitional/transitional_state.hpp"

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

ValidVar validCall(Call cal, CodeBlock *parent, bool inAssign){
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
         "You need to add call to exact element like '" + cyan + found->getName() + "(?)" + green + "'", cal.line, true);
    }

    if(!arrayCall){
        r.isConstant = found->isConstant();
        r.isInitialized = found->isInitialized();
        return r;
    }
    
    if(!cal.isFirstIndex){
        Call indexCall = Call();
        indexCall.line = cal.line;
        indexCall.name = cal.secondIdx;
        indexCall.isFirstIndex = false;
        auto isIdx = validInitializedCall(indexCall, parent);
        if(!isIdx.isInitialized){
            warning("Call to array " + cal.name + " by uninitialized variable", cal.line);
        }else if(isIdx.isConstant){
            auto i = declared[cal.secondIdx]->getValue();
            auto v = (ArrayVariable *)(declared[cal.name].get());
            try{
                auto el = v->getElement(i);
                bool isKnown = false;
                if(el != nullptr){
                    r.isConstant = el->isConstant();
                    r.isInitialized = el->isInitialized();
                    isKnown = true;
                }else{
                    r.isConstant = false;
                    r.isInitialized = true;
                    isKnown = false;
                }
                if(((isKnown && !r.isInitialized) || !isKnown) && !inAssign){
                    //todo sprawdzić bug
                    warning("Wanted element of array " + cal.name + " could be uninitialized", cal.line);
                }
            }catch(int i){
                warning("Call to element out of the array", cal.line);
            }
        }else{
            r.isConstant = false;
            r.isInitialized = true;
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
            r.isInitialized = true;
            return r;
        }
    }catch(int i){
        warning("Call to element out of the array", cal.line);
    }
    r.isConstant = false;
    r.isInitialized = false;
    return r;
}

ValidVar validInitializedCall(Call cal, CodeBlock *parent){
    return validCall(cal, parent, false);
}

ValidVar validVal(Value *val, CodeBlock *parent){
    
    if(val->cal != nullptr){
        auto initialized = validInitializedCall(*(val->cal.get()), parent);
        if(!initialized.isInitialized){                                      //todo 
            warning("Wanted value could be uninitialized",val->cal->line);
        }
        return initialized;
    }else{
        ValidVar r = ValidVar();
        r.isIterator = false;
        r.isConstant = true;
        r.isInitialized = true;
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
        for(unsigned int i=1; i<cmd->getNested().size(); i++){
            valid(cmd->getNested()[i].get());
        }
    }else if(cmd->getType() == CWHILE) {
        validWhile(cmd);
        for(unsigned int i=1; i<cmd->getNested().size(); i++){
            valid(cmd->getNested()[i].get());
        }
    }else if(cmd->getType() == CDOWHILE){
        for(unsigned int i=1; i<cmd->getNested().size(); i++){
            valid(cmd->getNested()[i].get());
        }
        validWhile(cmd);
    }
    
}

void validWrite(Command *cmd){
    auto c = cmd->getValue().cal;
    if(c != nullptr){
        auto r = validInitializedCall(*(c.get()), cmd);
        if(!r.isInitialized){
            warning("Attempting to WRITE value that could be uninitialized",cmd->getLastLine());
        }
    }
}

void validRead(Command *cmd){
    auto v = validCall(*(cmd->getCalls()[0].get()), cmd, true);
    if(v.isIterator){
        error("Attempting to change iterator",cmd->getLastLine(), true);
    }
    auto call = cmd->getCalls()[0];
    auto var = tree.getVariables()[call->name];
    if(var->isArray()){
        auto arr = (ArrayVariable *)(var.get());
        if(call->isFirstIndex){
            try{
                auto el = arr->getElement(call->firstIdx);
                if(el != nullptr){
                    el->setConstant(false);
                    el->setInitialized(true);
                }else{
                    std::string varName = call->name + ":" + std::to_string(call->firstIdx);
                    auto ell = std::make_shared<Variable>(varName);
                    ell->setConstant(false);
                    ell->setInitialized(true);
                    arr->setElement(call->firstIdx, ell);
                }
            }catch(int i){
                //warning already printed
            }
        }else{
            if(tree.getVariables()[call->name]->isConstant()){
                auto val = tree.getVariables()[call->name]->getValue();
                try{
                    auto arr = (ArrayVariable *)(tree.getVariables()[call->name].get());
                    var = arr->getElement(val);
                    if(var == nullptr){
                        std::string varName = call->name + ":" + call->secondIdx;
                        var = std::make_shared<Variable>(varName);
                        arr->setElement(val, var);
                        var->setConstant(false);
                        var->setInitialized(true);
                    }else{
                        var->setConstant(false);
                        var->setInitialized(true);
                    }
                }catch(int i){
                    //warning already printed
                }
            }else{
                var = nullptr;
            }
        }
    }else{
        log(var->toString());
        var->setInitialized(true);
        var->setConstant(false);
    }
}

void validAssign(Command *cmd){
    auto v = validCall(*(cmd->getCalls()[0].get()), cmd,true);
    if(v.isIterator){
        error("Attempting to change iterator",cmd->getLastLine(), true);
    }
    auto call = cmd->getCalls()[0];
    

    auto exp = std::dynamic_pointer_cast<Expression>(cmd->getNested()[0]);
    auto get = validExp(exp.get());
    auto name = call->name;
    std::shared_ptr<Variable> var;
    if(call->isFirstIndex){
        try{
            auto arr = (ArrayVariable *)(tree.getVariables()[name].get());
            var = arr->getElement(call->firstIdx);
            if(var == nullptr){
                std::string varName = call->name + ":" + std::to_string(call->firstIdx);
                var = std::make_shared<Variable>(varName);
                arr->setElement(call->firstIdx, var);
            }
        }catch(int i){
            //warning already printed
        }
    }else if(call->secondIdx != ""){
        if(tree.getVariables()[name]->isConstant()){
            auto val = tree.getVariables()[name]->getValue();
            try{
                auto arr = (ArrayVariable *)(tree.getVariables()[name].get());
                var = arr->getElement(val);
                if(var == nullptr){
                    std::string varName = call->name + ":" + call->secondIdx;
                    var = std::make_shared<Variable>(varName);
                    arr->setElement(val, var);
                }
            }catch(int i){
                //warning already printed
            }
        }else{
            var = nullptr;
        }
    }else{
        var = tree.getVariables()[name];
    }
    
    if(var != nullptr){
        if(get.isValue){
            if(!var->isInitialized()){
                var->setConstant(true);
                var->setValue(get.value);
            }else{
                var->setConstant(false);
                var->setValue(get.value);
            }
        }else{
            var->setConstant(false);
        }
        var->setInitialized(true);
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
            error ("variable " + name + " is already declared", true);
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
            if(left.isInitialized && left.isConstant){
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
        // if(!isInLoop(cond)){
        //     if(cond->getLeft().cal == nullptr){
        //         bool value;
        //         int64 l = cond->getLeft().val;
        //         int64 r;
        //         if(cond->getRight().cal == nullptr){
        //             r = cond->getRight().val;
        //             value = getCond(l, r, cond->getCond());
        //         }else{
        //             if(right.isConstant){
        //                 //todo if right.cal is constant;
        //             }
        //         }

        //         result.isConstVal = true;
        //         result.value = value;
        //     }else{
        //         if(left.isConstant){
        //             int64 l = 1; //todo get left.value
        //             if(cond->getRight().cal == nullptr){
        //                 int64 r = cond->getRight().val;
        //                 auto value = getCond(l, r, cond->getCond());
        //                 result.isConstVal = true;
        //                 result.value = value;
        //             }else{
        //                 if(right.isConstant){
        //                     //todo if right.cal is constant;
        //                 }
                        
        //             }
        //         }
        //         //todo if left.cal is constant
        //     }
        // }
    }

    return result;
}