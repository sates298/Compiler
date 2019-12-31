#ifndef BLOCKS_HPP
#define BLOCKS_HPP

#include<string>
#include<memory>
#include<map>
#include<vector>
#include<tuple>
#include "variables.hpp"
#include "assembler.hpp"

enum BlockType{
    ASSIGN, IF, IFELSE, WHILE, DOWHILE, FOR, FORDOWN, READ, WRITE
};

class CodeBlock{
    private:
        std::shared_ptr<CodeBlock> parent;
        BlockType type;
        std::vector<std::shared_ptr<CodeBlock>> nested;
        std::vector<std::shared_ptr<Asm>> assembler;

        bool forLoop;
        std::shared_ptr<Variable> iterator;
    public:
        CodeBlock( std::vector<std::shared_ptr<CodeBlock>> nestedBlocks, BlockType blockType);
        CodeBlock( std::shared_ptr<CodeBlock> nestedBlock, BlockType blockType);
        ~CodeBlock(){}

        void setForLoop(std::shared_ptr<Variable> iterator);
        std::shared_ptr<CodeBlock> getParent();
        BlockType getType();
        std::map<std::string, std::shared_ptr<Variable>> getLocalVariables();
        std::vector<std::shared_ptr<CodeBlock>> getNested();
        std::vector<std::shared_ptr<Asm>> getAssembler();
};
#endif