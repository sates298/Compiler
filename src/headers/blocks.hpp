#ifndef BLOCKS_HPP
#define BLOCKS_HPP

#include<string>
#include<memory>
#include<map>
#include<vector>
#include "variables.hpp"
#include "assembler.hpp"
#include "block_types.hpp"

class CodeBlock{
    protected:
        CodeBlock *parent;
        BlockType blockType;
        int64 startLine;
        std::vector<std::shared_ptr<Call>> calls;
        // asmVec assembler;
    public:
        CodeBlock(BlockType blockType, int64 startLine);
        virtual ~CodeBlock() = default;
        CodeBlock *getParent();
        void setParent(CodeBlock *parent);
        std::map<std::string, std::shared_ptr<Variable>> getLocalVariables();
        BlockType getBlockType();
        std::vector<std::shared_ptr<Call>> &getCalls();
        int64 getStartLine();
        // asmVec &getAssembler();


        std::string toString();
};

class Command: public CodeBlock{
    protected:
        std::vector<std::shared_ptr<CodeBlock>> nested;
        CommandType type;    
    private:   
        //to if else block
        int64 firstElseIndex;
        //to write block
        Value writeValue;
    public:
        Command(CommandType type, int64 startLine);
        Command(std::vector<std::shared_ptr<CodeBlock>> nested, CommandType type,int64 startLine);
        Command(std::shared_ptr<CodeBlock> nested, CommandType type,int64 startLine);
        virtual ~Command() = default;
        std::vector<std::shared_ptr<CodeBlock>> &getNested();
        void appendBlock(std::shared_ptr<CodeBlock> block);
        void appendBlocks(std::vector<std::shared_ptr<CodeBlock>> blocks);
        CommandType getType();

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
                 int64 startLine);
        ForLoop(std::string iterator, Value from, Value to,
                 std::vector<std::shared_ptr<CodeBlock>> nested,
                 CommandType type, int64 startLine);
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
        Expression(Value leftValue, ExpressionType expr, Value rightValue, int64 startLine);
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
        Condition(Value leftValue, ConditionType cond, Value rightValue, int64 startLine);
        virtual ~Condition() = default;
        bool isResultExist();
        bool getResult();
        Value &getLeft();
        ConditionType getCond();
        Value &getRight();

        std::string toString();
};

struct Multicommand{
    std::vector<std::shared_ptr<Command>> commands;
};

#endif