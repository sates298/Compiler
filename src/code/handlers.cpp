#include "../headers/handlers.hpp"

void handleVariableDeclaration(Variable var){
    std::string name = var.getName();
    auto it = tree.getVariables().find(name);
    if(it == tree.getVariables().end()){
        std::shared_ptr<Variable> newVar(new Variable(name));
        tree.getVariables()[name] = std::move(newVar);
    }else{
        error("variable " + name + " is already initialized", true);
    }
}
void handleArrayDeclaration(Variable var, long long first, long long last){
    std::string name = var.getName();
    auto it = tree.getVariables().find(name);
    if(it == tree.getVariables().end()){
        try{
            std::shared_ptr<Variable> newArr(new ArrayVariable(name, first, last));
            tree.getVariables()[name] = newArr;
        }catch(int i){
            error("array " + name + " has wrong boudns", "You could swap values of indexes like: " + name + "(" + std::to_string(last) + ":" + std::to_string(first) + ")",true);
        }
    }else{
        if(tree.getVariables()[name].get()->isArray()){
            error("array " + name + " is already initialized", true);
        }else{
            error("variable with name \"" + name + "\" is already initialized", true);
        }
    }
}

Call *handleName(std::string name){
    Call *returned = new Call();
    returned->name = name;
    return returned;
}
Call *handleArrayByName(std::string name, std::string idx){
    auto it = tree.getVariables().find(name);
    if(it == tree.getVariables().end()){
        error("Undefined array", yylineno, true);
        return nullptr;
    }

    Call *returned = new Call();
    returned->name = name;
    returned->isFirstIndex = false;
    returned->secondIdx = idx;
    return returned;
}
Call *handleArrayByNumber(std::string name, long long idx){
    auto it = tree.getVariables().find(name);
    if(it == tree.getVariables().end()){
        error("Undefined array", yylineno, true);
        return nullptr;
    }
    Call *returned = new Call();
    returned->name = name;
    returned->isFirstIndex = true;
    returned->firstIdx = idx;
    return returned;
}

Value *handleValueNumber(long long num){
    Value *returned = new Value();
    returned->val = num;
    return returned;
}
Value *handleValueIdentifier(Call cal){
    Value *returned = new Value();
    returned->cal = std::make_shared<Call>(cal);
    return returned;
}

Condition *handleCondition(Value left, Value right, ConditionType type){
    return new Condition(left, type, right);
}

Expression *handleExpression(Value left, Value right, ExpressionType type){
    return new Expression(left, type, right);
}

Command *handleAssign(Call a, Expression exp){
    Command *cmd = new Command(std::make_shared<Expression>(exp), CASSIGN);
    cmd->getCalls().emplace_back(std::make_shared<Call>(a));
    return cmd;
}
Command *handleIfElse(Condition cond, Multicommand *normal, Multicommand *elses){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CIFELSE);
    cmd->appendBlocks({normal->commands.begin(),normal->commands.end()});
    long long idx = (long long) cmd->getNested().size();
    cmd->appendBlocks({elses->commands.begin(),elses->commands.end()});
    cmd->setFirstElseIndex(idx);
    return cmd;
}
Command *handleIf(Condition cond, Multicommand *mcmd){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CIF);
    cmd->appendBlocks({mcmd->commands.begin(),mcmd->commands.end()});
    return cmd;
}
Command *handleWhile(Condition cond, Multicommand *mcmd){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CWHILE);
    cmd->appendBlocks({mcmd->commands.begin(),mcmd->commands.end()});
    return cmd;
}
Command *handleDoWhile(Condition cond, Multicommand *mcmd){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CDOWHILE);
    cmd->appendBlocks({mcmd->commands.begin(),mcmd->commands.end()});
    return cmd;
}
Command *handleFor(std::string iterator, Value from, Value to, Multicommand *mcmd){
    ForLoop *cmd =
             new ForLoop(iterator, from, to,
                        {mcmd->commands.begin(),mcmd->commands.end()}, CFOR);
        return cmd;
}
Command *handleForDown(std::string iterator, Value from, Value to, Multicommand *mcmd){
    ForLoop *cmd =
             new ForLoop(iterator, from, to,
                        {mcmd->commands.begin(),mcmd->commands.end()}, CFORDOWN);
    return cmd;
}
Command *handleRead(Call cal){
    Command *cmd = new Command(CREAD);
    cmd->getCalls().emplace_back(std::make_shared<Call>(cal));
    return cmd;
}
Command *handleWrite(Value val){
    Command *cmd = new Command(CWRITE);
    cmd->setValue(val);
    return cmd;
}

Multicommand *handleCommand(Command *cmd){
    Multicommand *mcmd = new Multicommand();
    if(cmd->getType() == CFOR || cmd->getType() == CFORDOWN){
        ForLoop *f = (ForLoop *)cmd;
        mcmd->commands.emplace_back(std::make_shared<ForLoop>(*f));
    }else{
        mcmd->commands.emplace_back(std::make_shared<Command>(*cmd));
    }
    return mcmd;
}
Multicommand *handleRecursiveCommands(Multicommand *mcmd, Command *cmd){
    if(cmd->getType() == CFOR || cmd->getType() == CFORDOWN){
        ForLoop *f = (ForLoop *)cmd;
        mcmd->commands.emplace_back(std::make_shared<ForLoop>(*f));
    }else{
        mcmd->commands.emplace_back(std::make_shared<Command>(*cmd));
    }
    return mcmd;
}

void handleProgram(Multicommand *mcmd){
    tree.setRoots(
        {mcmd->commands.begin(), mcmd->commands.end()}
    );
}