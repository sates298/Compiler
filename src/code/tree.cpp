#include "../headers/tree.hpp"

std::map<std::string, std::shared_ptr<Variable>> Tree::variables;
std::shared_ptr<CodeBlock> Tree::root;
std::vector<std::shared_ptr<Asm>> Tree::assembler;

void Tree::createTree(std::shared_ptr<CodeBlock> newRoot){
    Tree::root = newRoot;
}

std::map<std::string, std::shared_ptr<Variable>> Tree::getVariables(){
    return Tree::variables;
}

std::shared_ptr<CodeBlock> Tree::getRoot(){
    return Tree::root;
}

std::vector<std::shared_ptr<Asm>> Tree::getAssembler(){
    return Tree::assembler;
}