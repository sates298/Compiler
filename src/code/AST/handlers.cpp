#include "../../headers/AST/handlers.hpp"

void handleVariableDeclaration(std::string name){
    auto it = tree.getVariables().find(name);
    if(it == tree.getVariables().end()){
        std::shared_ptr<Variable> newVar = std::make_shared<Variable>(name);
        tree.getVariables()[name] = newVar;
    }else{
        error("variable " + name + " is already initialized", true);
    }
}
void handleArrayDeclaration(std::string name, int64 first, int64 last){
    auto it = tree.getVariables().find(name);
    if(it == tree.getVariables().end()){
        try{
            std::shared_ptr<ArrayVariable> newArr = std::make_shared<ArrayVariable>(name, first, last);
            tree.getVariables()[name] = newArr;
        }catch(int i){
            error("array " + name + " has wrong boudns", "You could swap values of indexes like: "+ cyan + name 
                    + "(" + std::to_string(last) + ":" + std::to_string(first) + ")",true);
        }
    }else{
        if(tree.getVariables()[name]->isArray()){
            error("array " + name + " is already initialized", true);
        }else{
            error("variable with name \"" + name + "\" is already initialized", true);
        }
    }
}

Call *handleName(std::string name){
    Call *returned = new Call();
    returned->line = yylineno;
    returned->name = name;
    return returned;
}
Call *handleArrayByName(std::string name, std::string idx){
    Call *returned = new Call();
    returned->line = yylineno;
    returned->name = name;
    returned->isFirstIndex = false;
    returned->secondIdx = idx;
    return returned;
}
Call *handleArrayByNumber(std::string name, int64 idx){
    Call *returned = new Call();
    returned->line = yylineno;
    returned->name = name;
    returned->isFirstIndex = true;
    returned->firstIdx = idx;
    return returned;
}

Value *handleValueNumber(int64 num){
    tree.getNumbers().insert(num);
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
    return new Condition(left, type, right, yylineno);
}

Expression *handleExpression(Value left, Value right, ExpressionType type){
    return new Expression(left, type, right,yylineno);
}

Command *handleAssign(Call a, Expression exp){
    Command *cmd = new Command(std::make_shared<Expression>(exp), CASSIGN, yylineno);
    cmd->getCalls().push_back(std::make_shared<Call>(a));
    return cmd;
}
Command *handleIfElse(Condition cond, Multicommand *normal, Multicommand *elses){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CIFELSE, yylineno);
    cmd->appendBlocks(normal->commands);
    int64 idx = (int64) cmd->getNested().size();
    cmd->appendBlocks(elses->commands);
    cmd->setFirstElseIndex(idx);
    return cmd;
}
Command *handleIf(Condition cond, Multicommand *mcmd){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CIF, yylineno);
    cmd->appendBlocks( mcmd->commands);
    return cmd;
}
Command *handleWhile(Condition cond, Multicommand *mcmd){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CWHILE, yylineno);
    cmd->appendBlocks( mcmd->commands);
    return cmd;
}
Command *handleDoWhile(Condition cond, Multicommand *mcmd){
    Command *cmd = new Command(std::make_shared<Condition>(cond), CDOWHILE, yylineno);
    cmd->appendBlocks( mcmd->commands);
    return cmd;
}
Command *handleFor(std::string iterator, Value from, Value to, Multicommand *mcmd){
    ForLoop *cmd =
             new ForLoop(iterator, from, to, mcmd->commands, CFOR, yylineno);
    return cmd;
}
Command *handleForDown(std::string iterator, Value from, Value to, Multicommand *mcmd){
    ForLoop *cmd =
             new ForLoop(iterator, from, to,mcmd->commands, CFORDOWN, yylineno);
    return cmd;
}
Command *handleRead(Call cal){
    Command *cmd = new Command(CREAD, yylineno);
    cmd->getCalls().push_back(std::make_shared<Call>(cal));
    return cmd;
}
Command *handleWrite(Value val){
    Command *cmd = new Command(CWRITE, yylineno);
    cmd->setValue(val);
    return cmd;
}

Multicommand *handleCommand(Command *cmd){
    Multicommand *mcmd = new Multicommand();
    if(cmd->getType() == CFOR || cmd->getType() == CFORDOWN){
        ForLoop *f = (ForLoop *)cmd;
        mcmd->commands.push_back(std::make_shared<ForLoop>(*f));
        
    }else{
        auto x = std::make_shared<Command>(*cmd);
        mcmd->commands.push_back(x);
    }
    return mcmd;
}
Multicommand *handleRecursiveCommands(Multicommand *mcmd, Command *cmd){
    if(cmd->getType() == CFOR || cmd->getType() == CFORDOWN){
        ForLoop *f = (ForLoop *)cmd;
        mcmd->commands.push_back(std::make_shared<ForLoop>(*f));
    }else{
        mcmd->commands.push_back(std::make_shared<Command>(*cmd));
    }
    return mcmd;
}

void handleProgram(Multicommand *mcmd){
    tree.setRoots(mcmd->commands);
    for(auto& b:tree.getRoots()){
        if(b->getBlockType() == CMD){
            auto tmp = std::dynamic_pointer_cast<Command>(b);
            tmp->setParentForAll();
        }
    }
}