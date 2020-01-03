#include "../headers/tree.hpp"

void Tree::setRoots(std::vector<std::shared_ptr<CodeBlock>> newRoots){
    this->roots = newRoots;
}

std::map<std::string, std::shared_ptr<Variable>> &Tree::getVariables(){
    return this->variables;
}

std::vector<std::shared_ptr<CodeBlock>> &Tree::getRoots(){
    return this->roots;
}

std::vector<std::shared_ptr<Asm>> &Tree::getAssembler(){
    return this->assembler;
}