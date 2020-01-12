#ifndef PSEUDO_GEN_HPP
#define PSEUDO_GEN_HPP

#include "transitional_state.hpp"
#include "support.hpp"

#define _PUSH_PSEUDO(a) {\
    code.emplace_back(a);\
    if(!waitingJumps[k].empty()){\
        for(auto& j:waitingJumps[k]){\
            j->setJumpReference(a);\
        }\
    }\
    k++;\
}
#define _PUSH(ins, reg) {\
    auto assm = std::make_shared<PseudoAsm>(k,ins,reg);\
    _PUSH_PSEUDO(assm);\
}
#define _WAIT_JUMP(a, b) {\
    waitingJumps[b].emplace_back(a);\
}

std::shared_ptr<PseudoAsm> pushJump(Instruction type);

void generate(CodeBlock *block);
void generateCmd(Command *cmd);

void generateAssign(Command *cmd);
void generateIf(Command *cmd);
void generateIfElse(Command *cmd);
void generateWhile(Command *cmd);
void generateDoWhile(Command *cmd);
void generateFor(ForLoop *fl);
void generateForDown(ForLoop *fl);
void generateRead(Command *cmd);
void generateWrite(Command *cmd);

void generateExpr(Expression *exp);

void generateNull(Expression *exp);
void generatePlus(Expression *exp);
void generateMinus(Expression *exp);
void generateTimes(Expression *exp);
void generateDiv(Expression *exp);
void generateMod(Expression *exp);

void generateCond(Condition *cond);

void generateEqual(Condition *cond);
void generateNotEqual(Condition *cond);
void generateLesser(Condition *cond);
void generateGreater(Condition *cond);
void generateLesserEqual(Condition *cond);
void generateGreaterEqual(Condition *cond);

void generatePseudoAsmByCallType(std::string call, Instruction instr);

void generatePseudoStore(std::string call);
void generatePseudoLoad(std::string call);
void generatePseudoSub(std::string call);
void generatePseudoAdd(std::string call);
void generatePseudoShift(std::string call);

void generatePseudoStorei(std::string call);
void generatePseudoLoadi(std::string call);
void generatePseudoSubi(std::string call);
void generatePseudoAddi(std::string call);
void generatePseudoShifti(std::string call);

#endif