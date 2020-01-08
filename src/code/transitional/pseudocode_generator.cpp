#include "../../headers/transitional/pseudocode_generator.hpp"

std::map<std::string, PseudoRegister> registers;
pseudoVec code; 
std::map<uint64, pseudoVec> waitingJumps; 

uint64 k = 0;

void generatePseudoCode(){
    for(auto& b: tree.getRoots()){
        generate(b.get());
    }
    auto halt = std::make_shared<PseudoAsm>(k, HALT, "null");
    _PUSH(halt);
}
void generateConstantsRegisters(){}

void generate(CodeBlock *block){
    switch(block->getBlockType()){
    case EXPR:
    {
        Expression *exp = (Expression *)(block);
        generateExpr(exp);
        break;
    }
    case CMD:
    {
        Command *cmd = (Command *)(block);
        generateCmd(cmd);
        break;
    }
    case COND:
    {
        Condition *cond = (Condition *)(block);
        generateCond(cond);
        break;
    }
    default:
        break;
    }
}

void generateCmd(Command *cmd){

    switch (cmd->getType()){
    case CFOR:
    {
        ForLoop *fl = (ForLoop *)(cmd);
        generateFor(fl);
        break;
    }
    case CFORDOWN:
    {
        ForLoop *fl = (ForLoop *)(cmd);
        generateForDown(fl);
        break;
    }
    case CWRITE:
        generateWrite(cmd);
        break;
    case CREAD:
        generateRead(cmd);
        break;
    case CASSIGN:
        generateAssign(cmd);
        break;
    case CIF:
        generateIf(cmd);
        break;
    case CIFELSE:
        generateIfElse(cmd);
        break;
    case CWHILE:
        generateWhile(cmd);
        break;
    case CDOWHILE:
        generateDoWhile(cmd);
        break;
    default:
        break;
    }
}

void generateAssign(Command *cmd){
    generate(cmd->getNested()[0].get());
    auto call = getCallName(*(cmd->getCalls()[0].get()));
    auto asgn = std::make_shared<PseudoAsm>(k, STORE, call);
    _PUSH(asgn);
}
void generateIf(Command *cmd){
    generate(cmd->getNested()[0].get());
    auto jump = code[k-1];
    for(unsigned long i=1; i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }
    _WAIT_JUMP(jump, k)
}
void generateIfElse(Command *cmd){
    generate(cmd->getNested()[0].get());
    auto jump = code[k-1];
    for(int64 i=1; i<cmd->getFirstElseIndex(); i++){
        generate(cmd->getNested()[i].get());
    }
    auto jElse = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(jElse);
    _WAIT_JUMP(jump, k);
    for(unsigned long i=cmd->getFirstElseIndex(); i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }

    // jElse->setJumpReference(code[k]); //todo na razie nie ma code[k]
    _WAIT_JUMP(jElse, k);
}
void generateWhile(Command *cmd){
    uint64 startK = k;
    generate(cmd->getNested()[0].get());
    auto jump = code[k-1];
    for(unsigned long i=1; i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }
    auto jWhile = std::make_shared<PseudoAsm>(k, JUMP, "null");
    jWhile->setJumpReference(code[startK]);
    _PUSH(jWhile);
    _WAIT_JUMP(jump, k);
    // jump->setJumpReference(code[k]);
}
void generateDoWhile(Command *cmd){
    auto jDoWhile = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(jDoWhile);
    uint64 startK = k;
    generate(cmd->getNested()[0].get());
    auto jump = code[k-1];
    _WAIT_JUMP(jDoWhile, k);
    for(unsigned long i=1; i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }
    auto jWhile = std::make_shared<PseudoAsm>(k, JUMP, "null");
    jWhile->setJumpReference(code[startK]);
    _PUSH(jWhile);
    _WAIT_JUMP(jump, k);
}
void generateFor(ForLoop *fl){
    //todo dodać jump do warunku
    auto it = fl->getIterator()->getName();
    auto from = getValueName(fl->getFrom()), to = getValueName(fl->getTo());

    auto loadFrom = std::make_shared<PseudoAsm>(k, LOAD, from);
    _PUSH(loadFrom);
    auto storeIt = std::make_shared<PseudoAsm>(k, STORE, it);
    _PUSH(storeIt);
    auto condJump = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(condJump);
    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }
    auto loadIt = std::make_shared<PseudoAsm>(k, LOAD, it);
    _PUSH(loadIt);
    auto inc = std::make_shared<PseudoAsm>(k, INC, "null");
    _PUSH(inc);

    condJump->setJumpReference(inc);

    auto storeIt1 = std::make_shared<PseudoAsm>(k, STORE, it);
    _PUSH(storeIt1);
    auto subTo = std::make_shared<PseudoAsm>(k, SUB, to);
    _PUSH(subTo);
    auto dec = std::make_shared<PseudoAsm>(k, DEC, "null");
    _PUSH(dec);
    auto jFor = std::make_shared<PseudoAsm>(k, JNEG, "null");
    jFor->setJumpReference(code[startK]);
    _PUSH(jFor);

}
void generateForDown(ForLoop *fl){
    //todo dodać jump do warunku
    auto it = fl->getIterator()->getName();
    auto from = getValueName(fl->getFrom()), to = getValueName(fl->getTo());

    auto loadFrom = std::make_shared<PseudoAsm>(k, LOAD, from);
    _PUSH(loadFrom);
    auto storeIt = std::make_shared<PseudoAsm>(k, STORE, it);
    _PUSH(storeIt);
    auto condJump = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(condJump);
    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }

    auto loadIt = std::make_shared<PseudoAsm>(k, LOAD, it);
    _PUSH(loadIt);
    auto dec = std::make_shared<PseudoAsm>(k, DEC, "null");
    _PUSH(dec);

    condJump->setJumpReference(dec);

    auto storeIt1 = std::make_shared<PseudoAsm>(k, STORE, it);
    _PUSH(storeIt1);
    auto subTo = std::make_shared<PseudoAsm>(k, SUB, to);
    _PUSH(subTo);
    auto inc = std::make_shared<PseudoAsm>(k, INC, "null");
    _PUSH(inc);
    auto jFor = std::make_shared<PseudoAsm>(k, JPOS, "null");
    jFor->setJumpReference(code[startK]);
    _PUSH(jFor);
}
void generateRead(Command *cmd){
    auto get = std::make_shared<PseudoAsm>(k, GET, "null");
    _PUSH(get);
    auto call = getCallName(*(cmd->getCalls()[0].get()));
    auto store = std::make_shared<PseudoAsm>(k, STORE, call);
    _PUSH(store);
}
void generateWrite(Command *cmd){
    auto val = cmd->getValue();
    auto call = getValueName(val);
    auto load = std::make_shared<PseudoAsm>(k, LOAD, call);
    _PUSH(load);

    auto put = std::make_shared<PseudoAsm>(k, PUT, "null");
    _PUSH(put);
}

void generateExpr(Expression *exp){

    if(exp->isResultExist()){
        int64 result = exp->getResult();
        auto loadResult = std::make_shared<PseudoAsm>(k, LOAD, std::to_string(result));
        _PUSH(loadResult);
        return;
    }
    switch (exp->getExpr())
    {
    case ENULL:
        generateNull(exp);
        break;
    case EPLUS:
        generatePlus(exp);
        break;
    case EMINUS:
        generateMinus(exp);
        break;
    case ETIMES:
        generateTimes(exp);
        break;
    case EDIV:
        generateDiv(exp);
        break;
    case EMOD:
        generateMod(exp);
        break;
    default:
        break;
    }
}

void generateNull(Expression *exp){
    auto name = getValueName(exp->getLeft());
    auto load = std::make_shared<PseudoAsm>(k, LOAD, name);
    _PUSH(load);
}
void generatePlus(Expression *exp){
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    auto load = std::make_shared<PseudoAsm>(k, LOAD, left);
    _PUSH(load);
    auto plus = std::make_shared<PseudoAsm>(k, ADD, right);
    _PUSH(plus);
}
void generateMinus(Expression *exp){
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    auto load = std::make_shared<PseudoAsm>(k, LOAD, left);
    _PUSH(load);
    auto minus = std::make_shared<PseudoAsm>(k, SUB, right);
    _PUSH(minus);
}
void generateTimes(Expression *exp){
    //todo
}
void generateDiv(Expression *exp){
    //todo
}
void generateMod(Expression *exp){
    //todo
}

void generateCond(Condition *cond){
    if(cond->isResultExist()){
        bool result = cond->getResult();
        if(result){
            auto jResult = std::make_shared<PseudoAsm>(k, JUMP, "null");
            _WAIT_JUMP(jResult, k+2);
            _PUSH(jResult);
        }
        auto jump = std::make_shared<PseudoAsm>(k, JUMP, "null");
        _PUSH(jump);
        return;
    }
    auto left = getValueName(cond->getLeft()), right = getValueName(cond->getRight());
    auto load = std::make_shared<PseudoAsm>(k, LOAD, left);
    _PUSH(load);
    auto minus = std::make_shared<PseudoAsm>(k, SUB, right);
    _PUSH(minus);

    switch (cond->getCond())
    {
    case CEQ:
        generateEqual(cond);
        break;
    case CNEQ:
        generateNotEqual(cond);
        break;
    case CLE:
        generateLesser(cond);
        break;
    case CGE:
        generateGreater(cond);
        break;
    case CLEQ:
        generateLesserEqual(cond);
        break;
    case CGEQ:
        generateGreaterEqual(cond);
        break;
    default:
        break;
    }
}

void generateEqual(Condition *cond){
    auto jtrue = std::make_shared<PseudoAsm>(k, JZERO, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH(jtrue);
    auto jfalse = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(jfalse);
}
void generateNotEqual(Condition *cond){
    auto jfalse = std::make_shared<PseudoAsm>(k, JZERO, "null");
    _PUSH(jfalse);
}
void generateLesser(Condition *cond){
    auto jtrue = std::make_shared<PseudoAsm>(k, JNEG, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH(jtrue);
    auto jfalse = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(jfalse);
}
void generateGreater(Condition *cond){
    auto jtrue = std::make_shared<PseudoAsm>(k, JPOS, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH(jtrue);
    auto jfalse = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(jfalse);
}
void generateLesserEqual(Condition *cond){
    auto jfalse = std::make_shared<PseudoAsm>(k, JPOS, "null");
    _PUSH(jfalse);
}
void generateGreaterEqual(Condition *cond){
    auto jfalse = std::make_shared<PseudoAsm>(k, JNEG, "null");
    _PUSH(jfalse);
}