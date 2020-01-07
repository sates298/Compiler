#include "../../headers/AST/tree.hpp"

void Tree::setRoots(std::vector<std::shared_ptr<CodeBlock>> newRoots){
    this->roots = newRoots;
}

std::map<std::string, std::shared_ptr<Variable>> &Tree::getVariables(){
    return this->variables;
}

std::set<int64> &Tree::getNumbers(){
    return this->numbers;
}

std::vector<std::shared_ptr<CodeBlock>> &Tree::getRoots(){
    return this->roots;
}