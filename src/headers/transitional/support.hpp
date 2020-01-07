#ifndef SUPPORT_HPP
#define SUPPORT_HPP

#include "../global.hpp"

bool getCond(int64 left, int64 right, ConditionType type);
int64 getValueFromConstant(Value v);
bool isInLoop(CodeBlock *c);

std::string getCallName(Call c);
#endif