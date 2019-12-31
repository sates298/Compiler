#ifndef TREE_HPP
#define TREE_HPP

#include "blocks.hpp"

class Tree{
    private:
        static std::map<std::string, std::shared_ptr<Variable>> variables;
        static std::shared_ptr<CodeBlock> root;
        static std::vector<std::shared_ptr<Asm>> assembler;
    public:
        static void createTree(std::shared_ptr<CodeBlock> newRoot);
        static std::map<std::string, std::shared_ptr<Variable>> getVariables();
        static std::shared_ptr<CodeBlock> getRoot();
        static std::vector<std::shared_ptr<Asm>> getAssembler();
};

#endif