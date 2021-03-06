#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "tree.hpp"
#include "../errors.hpp"

extern int yylineno;

void handleVariableDeclaration(std::string name);
void handleArrayDeclaration(std::string name, int64 first, int64 last);

Call *handleName(std::string name);
Call *handleArrayByName(std::string name, std::string idx);
Call *handleArrayByNumber(std::string name, int64 idx);

Value *handleValueNumber(int64 num);
Value *handleValueIdentifier(Call cal);

Condition *handleCondition(Value left, Value right, ConditionType type);

Expression *handleExpression(Value left, Value right, ExpressionType type);

Command *handleAssign(Call a, Expression exp);
Command *handleIfElse(Condition cond, Multicommand *normal, Multicommand *elses);
Command *handleIf(Condition cond, Multicommand *mcmd);
Command *handleWhile(Condition cond, Multicommand *mcmd);
Command *handleDoWhile(Condition cond, Multicommand *mcmd);
Command *handleFor(std::string iterator, Value from, Value to, Multicommand *mcmd);
Command *handleForDown(std::string iterator, Value from, Value to, Multicommand *mcmd);
Command *handleRead(Call Call);
Command *handleWrite(Value val);

Multicommand *handleCommand(Command *cmd);
Multicommand *handleRecursiveCommands(Multicommand *mcmd, Command *cmd);

void handleProgram(Multicommand *mcmd);

#endif