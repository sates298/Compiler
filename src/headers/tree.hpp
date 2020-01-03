#ifndef TREE_HPP
#define TREE_HPP

#include "blocks.hpp"

class Tree{
    private:
        std::map<std::string, std::shared_ptr<Variable>> variables;
        std::shared_ptr<CodeBlock> root;
        std::vector<std::shared_ptr<Asm>> assembler;
    public:
        Tree(){};
        void setRoot(CodeBlock *newRoot);
        std::map<std::string, std::shared_ptr<Variable>> getVariables();
        std::shared_ptr<CodeBlock> getRoot();
        std::vector<std::shared_ptr<Asm>> getAssembler();
};

std::shared_ptr<Tree> tree = std::make_shared<Tree>();
Tree tree2 = Tree();

#endif