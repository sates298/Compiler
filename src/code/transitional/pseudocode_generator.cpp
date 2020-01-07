#include "../../headers/transitional/pseudocode_generator.hpp"

std::map<std::string, PseudoRegister> registers;
std::vector<std::shared_ptr<PseudoAsm>> code; 
uint64 k = 0;


void generatePseudoCode(){
    for(auto& b: tree.getRoots()){
        generate(b.get(), false);
    }
}
void generateConstantsRegisters();

void generate(CodeBlock *block, bool inJump){
    switch(block->getBlockType()){
    case EXPR:
        Expression *exp = (Expression *)(block);
        generateExpr(exp, inJump);
    case CMD:
        Command *cmd = (Command *)(block);
        generateCmd(cmd, inJump);
    case COND:
        Condition *cond = (Condition *)(block);
        generateCond(cond, inJump);
        break;
    default:
        break;
    }
}

void generateCmd(Command *cmd, bool inJump){

    switch (cmd->getType()){
    case CFOR:
        ForLoop *fl = (ForLoop *)(cmd);
        generateFor(fl, inJump);
        break;
    case CFORDOWN:
        ForLoop *fl = (ForLoop *)(cmd);
        generateForDown(fl, inJump);
        break;
    case CWRITE:
        generateWrite(cmd, inJump);
        break;
    case CREAD:
        generateRead(cmd, inJump);
        break;
    case CASSIGN:
        generateAssign(cmd, inJump);
        break;
    case CIF:
        generateIf(cmd, inJump);
        break;
    case CIFELSE:
        generateIfElse(cmd, inJump);
        break;
    case CWHILE:
        generateWhile(cmd, inJump);
        break;
    case CDOWHILE:
        generateDoWhile(cmd, inJump);
        break;
    default:
        break;
    }
}


void generateAssign(Command *cmd, bool inJump){
    generate(cmd->getNested()[0].get(), inJump);
    auto call = getCallName(*(cmd->getCalls()[0].get()));
    auto asgn = std::make_shared<PseudoAsm>(k, STORE, call, inJump);
    _PUSH(asgn);
    k++;
}
void generateIf(Command *cmd, bool inJump);
void generateIfElse(Command *cmd, bool inJump);
void generateWhile(Command *cmd, bool inJump);
void generateDoWhile(Command *cmd, bool inJump);
void generateFor(ForLoop *fl, bool inJump);
void generateForDown(ForLoop *fl, bool inJump);
void generateRead(Command *cmd, bool inJump);
void generateWrite(Command *cmd, bool inJump);

void generateExpr(Expression *exp, bool inJump){
    switch (exp->getExpr())
    {
    case ENULL:
        generateNull(exp, inJump);
        break;
    case EPLUS:
        generatePlus(exp, inJump);
        break;
    case EMINUS:
        generateMinus(exp, inJump);
        break;
    case ETIMES:
        generateTimes(exp, inJump);
        break;
    case EDIV:
        generateDiv(exp, inJump);
        break;
    case EMOD:
        generateMod(exp, inJump);
        break;
    default:
        break;
    }
}

void generateNull(Expression *exp, bool inJump);
void generatePlus(Expression *exp, bool inJump);
void generateMinus(Expression *exp, bool inJump);
void generateTimes(Expression *exp, bool inJump);
void generateDiv(Expression *exp, bool inJump);
void generateMod(Expression *exp, bool inJump);

void generateCond(Condition *cond, bool inJump){
    switch (cond->getCond())
    {
    case CEQ:
        generateEqual(cond, inJump);
        break;
    case CNEQ:
        generateNotEqual(cond, inJump);
        break;
    case CLE:
        generateLesser(cond, inJump);
        break;
    case CGE:
        generateGreater(cond, inJump);
        break;
    case CLEQ:
        generateLesserEqual(cond, inJump);
        break;
    case CGEQ:
        generateGreaterEqual(cond, inJump);
        break;
    default:
        break;
    }
}

void generateEqual(Condition *cond, bool inJump);
void generateNotEqual(Condition *cond, bool inJump);
void generateLesser(Condition *cond, bool inJump);
void generateGreater(Condition *cond, bool inJump);
void generateLesserEqual(Condition *cond, bool inJump);
void generateGreaterEqual(Condition *cond, bool inJump);
