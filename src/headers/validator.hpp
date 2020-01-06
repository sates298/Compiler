#ifndef VALID_HPP
#define VALID_HPP

#include "global.hpp"
#include "errors.hpp"

void valid();
void valid(CodeBlock *block);
void valid(Call cal, CodeBlock *parent);
void valid(Value *val, CodeBlock *parent);
void valid(Command *cmd);
void valid(ForLoop *forl);
void valid(Expression *expr);
void valid(Condition *cond);

#endif