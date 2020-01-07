#ifndef VALID_HPP
#define VALID_HPP

#include "support.hpp"
#include "../errors.hpp"

struct ValidVar{
    bool isDeclared;
    bool isConstant;

    bool isIterator;
};

struct ValidExp{
    bool isValue;
    int64 value;
};

struct ValidCond{
    bool isConstVal;
    bool value;
};

void valid();
void valid(CodeBlock *block);
ValidVar validDeclaredCall(Call cal, CodeBlock *parent);
ValidVar validVal(Value val, CodeBlock *parent);
void valid(Command *cmd);
void validWrite(Command *cmd);
void validRead(Command *cmd);
void validAssign(Command *cmd);
void validIf(Command *cmd);
void validWhile(Command *cmd);
void validFor(ForLoop *f);
ValidExp validExp(Expression *expr);
ValidCond validCond(Condition *cond);



#endif