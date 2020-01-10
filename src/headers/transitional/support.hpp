#ifndef SUPPORT_HPP
#define SUPPORT_HPP

#include "../AST/tree.hpp"

bool getCond(int64 left, int64 right, ConditionType type);
int64 getValueFromConstant(Value v);
bool isInLoop(CodeBlock *c);

std::string getCallName(Call c);
std::string getValueName(Value val);
#endif