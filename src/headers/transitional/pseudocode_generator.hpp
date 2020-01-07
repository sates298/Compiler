#ifndef PSEUDO_GEN_HPP
#define PSEUDO_GEN_HPP

#include "transitional_state.hpp"
#include "support.hpp"

#define _PUSH(a) code.emplace_back(a)

void generatePseudoCode();
void generateConstantsRegisters();

void generate(CodeBlock *block, bool inJump);

void generateCmd(Command *cmd, bool inJump);

void generateAssign(Command *cmd, bool inJump);
void generateIf(Command *cmd, bool inJump);
void generateIfElse(Command *cmd, bool inJump);
void generateWhile(Command *cmd, bool inJump);
void generateDoWhile(Command *cmd, bool inJump);
void generateFor(ForLoop *fl, bool inJump);
void generateForDown(ForLoop *fl, bool inJump);
void generateRead(Command *cmd, bool inJump);
void generateWrite(Command *cmd, bool inJump);

void generateExpr(Expression *exp, bool inJump);

void generateNull(Expression *exp, bool inJump);
void generatePlus(Expression *exp, bool inJump);
void generateMinus(Expression *exp, bool inJump);
void generateTimes(Expression *exp, bool inJump);
void generateDiv(Expression *exp, bool inJump);
void generateMod(Expression *exp, bool inJump);

void generateCond(Condition *cond, bool inJump);

void generateEqual(Condition *cond, bool inJump);
void generateNotEqual(Condition *cond, bool inJump);
void generateLesser(Condition *cond, bool inJump);
void generateGreater(Condition *cond, bool inJump);
void generateLesserEqual(Condition *cond, bool inJump);
void generateGreaterEqual(Condition *cond, bool inJump);

#endif