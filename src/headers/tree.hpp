#ifndef TREE_HPP
#define TREE_HPP

#include "blocks.hpp"

class Tree{
    private:
        std::map<std::string, std::shared_ptr<Variable>> variables;
        std::vector<std::shared_ptr<CodeBlock>> roots;
        std::vector<std::shared_ptr<Asm>> assembler;
    public:
        Tree(){};
        void setRoots(std::vector<std::shared_ptr<CodeBlock>> newRoots);
        std::map<std::string, std::shared_ptr<Variable>> &getVariables();
        std::vector<std::shared_ptr<CodeBlock>> &getRoots();
        std::vector<std::shared_ptr<Asm>> &getAssembler();
};
#endif