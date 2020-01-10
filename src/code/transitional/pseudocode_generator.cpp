#include "../../headers/transitional/pseudocode_generator.hpp"

std::map<std::string, std::shared_ptr<PseudoRegister>> registers;
pseudoVec code; 
std::map<uint64, pseudoVec> waitingJumps; 

uint64 k = 0;

void generatePseudoRegisters(){
    for(auto n:tree.getNumbers()){
        std::string name = std::to_string(n);
        auto pseudo = std::make_shared<PseudoRegister>();
        pseudo->name = name;
        pseudo->isNumber = true;
        registers[name] = pseudo;
    }
    for(auto& [k,v]:tree.getVariables()){
        std::string name = v->getName();
        auto pseudo = std::make_shared<PseudoRegister>();
        pseudo->name = name;
        if(v->isArray()){
            pseudo->isArray = true;
        }
        pseudo->var = v.get();
        registers[name] = pseudo;
    }   
}

void generatePseudoCode(){
    for(auto& b: tree.getRoots()){
        generate(b.get());
    }
    auto halt = std::make_shared<PseudoAsm>(k, HALT, "null");
    _PUSH(halt);
}

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
    
   generatePseudoAsmByCallType(call, STORE);
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
    auto it = fl->getIterator()->getName();
    auto from = getValueName(fl->getFrom()), to = getValueName(fl->getTo());

    auto pseudo = std::make_shared<PseudoRegister>();
    pseudo->isIterator = true;
    pseudo->name = it;
    registers[it] = pseudo;

    generatePseudoAsmByCallType(from, LOAD);
    auto condJump = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(condJump);
    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }
    generatePseudoAsmByCallType(it, LOAD);
    auto inc = std::make_shared<PseudoAsm>(k, INC, "null");
    _PUSH(inc);

    _WAIT_JUMP(condJump, k);

    generatePseudoAsmByCallType(it, STORE); //it'll be always normal store - this is iterator
    generatePseudoAsmByCallType(to, SUB);
    auto dec = std::make_shared<PseudoAsm>(k, DEC, "null");
    _PUSH(dec);
    auto jFor = std::make_shared<PseudoAsm>(k, JNEG, "null");
    jFor->setJumpReference(code[startK]);
    _PUSH(jFor);

}
void generateForDown(ForLoop *fl){
    auto it = fl->getIterator()->getName();
    auto from = getValueName(fl->getFrom()), to = getValueName(fl->getTo());

    auto pseudo = std::make_shared<PseudoRegister>();
    pseudo->isIterator = true;
    pseudo->name = it;
    registers[it] = pseudo;
    generatePseudoAsmByCallType(from, LOAD);
    generatePseudoAsmByCallType(it, STORE);
    auto condJump = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH(condJump);
    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }
    generatePseudoAsmByCallType(it, LOAD);
    auto dec = std::make_shared<PseudoAsm>(k, DEC, "null");
    _PUSH(dec);
    _WAIT_JUMP(condJump, k);
    generatePseudoAsmByCallType(it, STORE); //it'll be always normal store - this is iterator
    generatePseudoAsmByCallType(to, SUB);
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
    
    generatePseudoAsmByCallType(call, STORE);
}
void generateWrite(Command *cmd){
    auto val = cmd->getValue();
    auto call = getValueName(val);
    generatePseudoAsmByCallType(call, LOAD);

    auto put = std::make_shared<PseudoAsm>(k, PUT, "null");
    _PUSH(put);
}

void generateExpr(Expression *exp){

    if(exp->isResultExist() && exp->getExpr() != EPLUS && exp->getExpr() != EMINUS){
        int64 result = exp->getResult();
        auto n = std::to_string(result);
        generatePseudoAsmByCallType(n, LOAD);
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
    generatePseudoAsmByCallType(name, LOAD);
}
void generatePlus(Expression *exp){
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    generatePseudoAsmByCallType(left, LOAD);
    generatePseudoAsmByCallType(right, ADD);    
}
void generateMinus(Expression *exp){
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    generatePseudoAsmByCallType(left, LOAD);
    generatePseudoAsmByCallType(right, SUB);    
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
        auto jump = std::make_shared<PseudoAsm>(k, JUMP, "null"); //todo zrobić, żeby pseudoAsm wiedział, że wskazuje na niego jump
        _PUSH(jump);
        return;
    }
    auto left = getValueName(cond->getLeft()), right = getValueName(cond->getRight());
    generatePseudoAsmByCallType(left, LOAD);
    generatePseudoAsmByCallType(right, SUB);    
    
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

void generatePseudoAsmByCallType(std::string call, Instruction instr){
    auto it = call.find(":");
    bool array;
    if(it == std::string::npos){
        array = false;
    }else{
        array = true;
    }
    switch(instr){
        case STORE:
            {array ? generatePseudoStorei(call) : generatePseudoStore(call);}
            break;
        case LOAD:
            {array ? generatePseudoLoadi(call) : generatePseudoLoad(call);}
            break;
        case ADD:
            {array ? generatePseudoAddi(call) : generatePseudoAdd(call);}
            break;
        case SUB:
            {array ? generatePseudoSubi(call) : generatePseudoSub(call);}
            break;
        case SHIFT:
            {array ? generatePseudoShifti(call) : generatePseudoShift(call);}
            break;
        default:
            break;
    }
}

void generatePseudoStore(std::string call){
    auto pseudo = std::make_shared<PseudoAsm>(k, STORE, call);
    _PUSH(pseudo);
}
void generatePseudoLoad(std::string call){
    auto pseudo = std::make_shared<PseudoAsm>(k, LOAD, call);
    _PUSH(pseudo)
}
void generatePseudoSub(std::string call){
    auto pseudo = std::make_shared<PseudoAsm>(k, SUB, call);
    _PUSH(pseudo)
}
void generatePseudoAdd(std::string call){
    auto pseudo = std::make_shared<PseudoAsm>(k, ADD, call);
    _PUSH(pseudo)
}
void generatePseudoShift(std::string call){
    auto pseudo = std::make_shared<PseudoAsm>(k, SHIFT, call);
    _PUSH(pseudo)
}

void generatePseudoStorei(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    auto storeTmp = std::make_shared<PseudoAsm>(k, STORE, "TMP");
    _PUSH(storeTmp)
    auto loadIdx = std::make_shared<PseudoAsm>(k, LOAD, idx);
    _PUSH(loadIdx)
    auto addOff = std::make_shared<PseudoAsm>(k, ADD, arr+"-off");
    _PUSH(addOff)
    auto storeTmp2 = std::make_shared<PseudoAsm>(k, STORE, "TMP2");
    _PUSH(storeTmp2)
    auto loadTmp = std::make_shared<PseudoAsm>(k, LOAD, "TMP");
    _PUSH(loadTmp)
    auto storei = std::make_shared<PseudoAsm>(k, STOREI, "TMP2");
    _PUSH(storei)  
}
void generatePseudoLoadi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    auto loadIdx = std::make_shared<PseudoAsm>(k, LOAD, idx);
    _PUSH(loadIdx)
    auto addOff = std::make_shared<PseudoAsm>(k, ADD, arr+"-off");
    _PUSH(addOff)
    auto loadi = std::make_shared<PseudoAsm>(k, LOADI, "acc");
    _PUSH(loadi)   
}
void generatePseudoSubi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    auto storeTmp = std::make_shared<PseudoAsm>(k, STORE, "TMP");
    _PUSH(storeTmp)
    auto loadIdx = std::make_shared<PseudoAsm>(k, LOAD, idx);
    _PUSH(loadIdx)
    auto addOff = std::make_shared<PseudoAsm>(k, ADD, arr+"-off");
    _PUSH(addOff)
    auto loadi = std::make_shared<PseudoAsm>(k, LOADI, "acc");
    _PUSH(loadi)
    auto storeTmp2 = std::make_shared<PseudoAsm>(k, STORE, "TMP2");
    _PUSH(storeTmp2)
    auto loadTmp = std::make_shared<PseudoAsm>(k, LOAD, "TMP");
    _PUSH(loadTmp)
    auto subTmp2 = std::make_shared<PseudoAsm>(k, SUB, "TMP2");
    _PUSH(subTmp2)
}
void generatePseudoAddi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    auto storeTmp = std::make_shared<PseudoAsm>(k, STORE, "TMP");
    _PUSH(storeTmp)
    auto loadIdx = std::make_shared<PseudoAsm>(k, LOAD, idx);
    _PUSH(loadIdx)
    auto addOff = std::make_shared<PseudoAsm>(k, ADD, arr+"-off");
    _PUSH(addOff)
    auto loadi = std::make_shared<PseudoAsm>(k, LOADI, "acc");
    _PUSH(loadi)
    auto addTmp = std::make_shared<PseudoAsm>(k, ADD, "TMP");
    _PUSH(addTmp)
}
void generatePseudoShifti(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    auto storeTmp = std::make_shared<PseudoAsm>(k, STORE, "TMP");
    _PUSH(storeTmp)
    auto loadIdx = std::make_shared<PseudoAsm>(k, LOAD, idx);
    _PUSH(loadIdx)
    auto addOff = std::make_shared<PseudoAsm>(k, ADD, arr+"-off");
    _PUSH(addOff)
    auto loadi = std::make_shared<PseudoAsm>(k, LOADI, "acc");
    _PUSH(loadi)
    auto storeTmp2 = std::make_shared<PseudoAsm>(k, STORE, "TMP2");
    _PUSH(storeTmp2)
    auto loadTmp = std::make_shared<PseudoAsm>(k, LOAD, "TMP");
    _PUSH(loadTmp)
    auto shiftTmp2 = std::make_shared<PseudoAsm>(k, SHIFT, "TMP2");
    _PUSH(shiftTmp2)
}
