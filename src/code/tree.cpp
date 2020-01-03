#include "../headers/tree.hpp"

void Tree::setRoot(CodeBlock *newRoot){
    this->root = std::make_shared<CodeBlock>(newRoot);
}

std::map<std::string, std::shared_ptr<Variable>> Tree::getVariables(){
    return this->variables;
}

std::shared_ptr<CodeBlock> Tree::getRoot(){
    return this->root;
}

std::vector<std::shared_ptr<Asm>> Tree::getAssembler(){
    return this->assembler;
}