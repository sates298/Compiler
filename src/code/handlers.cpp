#include "../headers/handlers.hpp"

void handleVariableDeclaration(std::string name){
    std::map<std::string, std::shared_ptr<Variable>>::iterator it = tree.get()->getVariables().find(name);
    tree2.getVariables();
    if(it != tree.get()->getVariables().end()){
        std::shared_ptr<Variable> newVar = std::make_shared<Variable>(name);
        tree.get()->getVariables()[name] = newVar;
    }else{
        error("variable " + name + " is already initialized");
    }
}
void handleArrayDeclaration(std::string name, long long first, long long last){
    std::map<std::string, std::shared_ptr<Variable>>::iterator it = tree.get()->getVariables().find(name);

    if(it != tree.get()->getVariables().end()){
        std::shared_ptr<Variable> newArr = std::make_shared<ArrayVariable>(name, first, last);
        tree.get()->getVariables()[name] = newArr;
    }else{
        error("variable " + name + " is already initialized");
    }
}