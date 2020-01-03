#ifndef BLOCKS_HPP
#define BLOCKS_HPP

#include<string>
#include<memory>
#include<map>
#include<vector>
#include<tuple>
#include "variables.hpp"
#include "assembler.hpp"
#include "block_types.hpp"

class CodeBlock{
    private:
        std::shared_ptr<CodeBlock> parent;
        BlockType blockType;
        // std::vector<std::shared_ptr<Asm>> assembler;
    public:
        CodeBlock(BlockType blockType);
        ~CodeBlock(){};
        std::shared_ptr<CodeBlock> getParent();
        void setParent(std::shared_ptr<CodeBlock> parent);
        std::map<std::string, std::shared_ptr<Variable>> getLocalVariables();
        BlockType getBlockType();
        // std::vector<std::shared_ptr<Asm>> getAssembler();
};

class Command: public CodeBlock{
    private:
        std::vector<std::shared_ptr<CodeBlock>> nested;
        CommandType type;
        std::shared_ptr<Variable> iterator;

        
    public:
        Command(std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type);
        Command(std::shared_ptr<CodeBlock> nested, CommandType type);
        ~Command(){};
        std::vector<std::shared_ptr<CodeBlock>> getNested();
        void setForLoop(std::shared_ptr<Variable> iterator);
        CommandType getType();
        std::shared_ptr<Variable> getIterator();
};


class Expression : public CodeBlock{
    private:
        value leftValue;
        ExpressionType expr;
        value rightValue;

        bool resultExists = false;
        long long result;

        void checkResult();
        void computeResult(long long a, long long b);
    public:
        Expression(value leftValue, ExpressionType expr, value rightValue);
        bool isResultExist();
        long long getResult();
        value getLeft();
        ExpressionType getExpr();
        value getRight();
};

class Condition : public CodeBlock{
    private:
        value leftValue;
        ConditionType cond;
        value rightValue;

        bool resultExists = false;
        bool result;
        
        void checkResult();
        void computeResult(long long a, long long b);
    public:
        Condition(value leftValue, ConditionType cond, value rightValue);
        bool isResultExist();
        bool getResult();
        value getLeft();
        ConditionType getCond();
        value getRight();
};

#endif