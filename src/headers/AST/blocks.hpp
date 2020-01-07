#ifndef BLOCKS_HPP
#define BLOCKS_HPP

#include<string>
#include<memory>
#include<map>
#include<vector>
#include "variables.hpp"
#include "block_types.hpp"

class CodeBlock {
    protected:
        std::weak_ptr<CodeBlock> parent;
        BlockType blockType;
        int64 lastLine;
        std::vector<std::shared_ptr<Call>> calls;
    public:
        CodeBlock(BlockType blockType, int64 lastLine);
        virtual ~CodeBlock() = default;
        std::weak_ptr<CodeBlock> getParent();
        void setParent(std::shared_ptr<CodeBlock> parent);
        std::map<std::string, std::shared_ptr<Variable>> getLocalVariables();
        BlockType getBlockType();
        std::vector<std::shared_ptr<Call>> &getCalls();
        int64 getLastLine();

        std::string toString();
};

class Command: public CodeBlock, public std::enable_shared_from_this<Command>{
    protected:
        std::vector<std::shared_ptr<CodeBlock>> nested;
        CommandType type;    
    private:   
        //to if else block
        int64 firstElseIndex;
        //to write block
        Value writeValue;
    public:
        Command(CommandType type, int64 lastLine);
        Command(std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type,int64 lastLine);
        Command(std::shared_ptr<CodeBlock> nested, CommandType type,int64 lastLine);
        virtual ~Command() = default;
        std::vector<std::shared_ptr<CodeBlock>> &getNested();
        CommandType getType();

        void appendBlocks(std::vector<std::shared_ptr<CodeBlock>> blocks);

        void setParentForAll();
        //to if else block
        int64 getFirstElseIndex();
        void setFirstElseIndex(int64 idx);
        //to write block
        Value &getValue();
        void setValue(Value val);

        std::string toString();
};

class ForLoop : public Command{
    private:
        std::shared_ptr<Variable> iterator;
        Value from;
        Value to;
    public:
        ForLoop(std::string iterator, Value from, Value to,
                 std::shared_ptr<CodeBlock> nested, CommandType type,
                 int64 lastLine);
        ForLoop(std::string iterator, Value from, Value to,
                 std::vector<std::shared_ptr<CodeBlock>> nested,
                 CommandType type, int64 lastLine);
        virtual ~ForLoop() = default;
        std::shared_ptr<Variable> getIterator();
        Value &getFrom();
        Value &getTo();

        std::string toString();
};

class Expression : public CodeBlock{
    private:
        Value leftValue;
        ExpressionType expr;
        Value rightValue;

        bool resultExists = false;
        int64 result;

        // void checkResult();
        // void computeResult(int64 a, int64 b);
    public:
        Expression(Value leftValue, ExpressionType expr, Value rightValue, int64 lastLine);
        virtual ~Expression() = default;
        bool isResultExist();
        int64 getResult();
        Value &getLeft();
        ExpressionType getExpr();
        Value &getRight();

        std::string toString();
};

class Condition : public CodeBlock{
    private:
        Value leftValue;
        ConditionType cond;
        Value rightValue;

        bool resultExists = false;
        bool result;
        
        // void checkResult();
        // void computeResult(int64 a, int64 b);
    public:
        Condition(Value leftValue, ConditionType cond, Value rightValue, int64 lastLine);
        virtual ~Condition() = default;
        bool isResultExist();
        bool getResult();
        Value &getLeft();
        ConditionType getCond();
        Value &getRight();

        std::string toString();
};

struct Multicommand{
    std::vector<std::shared_ptr<CodeBlock>> commands;
};

#endif