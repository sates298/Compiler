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

            auto arr = (ArrayVariable *)v.get();
            auto from = arr->getFrom(), to = arr->getTo();
            int64 size = to - from + 1;
            pseudo->sizeArr = size;
            auto offset = std::make_shared<PseudoRegister>();
            offset->isNumber = true;
            offset->isOffset = true;
            offset->offsetVal = -from;
            offset->name = pseudo->name + "-off";
            registers[offset->name] = offset;

        }
        pseudo->var = v.get();
        registers[name] = pseudo;
    }
    //create pseudo accumulator
    auto acc = std::make_shared<PseudoRegister>();
    acc->index = 0;
    acc->name = "ACC";
    registers[acc->name] = acc;
    //create needed values
    auto one = std::make_shared<PseudoRegister>();
    one->name = "1";
    one->isNumber = true;
    registers[one->name] = one;
    auto mone = std::make_shared<PseudoRegister>();
    mone->name = "-1";
    mone->isNumber = true;
    registers[mone->name] = mone;
    //creating memory to temporary values in calls to array
    auto tmp = std::make_shared<PseudoRegister>();
    tmp->name = "TMParr";
    registers[tmp->name] = tmp;

    auto tmp2 = std::make_shared<PseudoRegister>();
    tmp2->name = "TMP2arr";
    registers[tmp2->name] = tmp2;
    //create memory to temporary values in expressions
    for(int i=1; i<7; i++){
        auto tmpExp = std::make_shared<PseudoRegister>();
        tmpExp->name = "TMP"+std::to_string(i)+"exp";
        registers[tmpExp->name] = tmpExp;
    }
    auto tmpLeft = std::make_shared<PseudoRegister>();
    tmpLeft->name = "TMPleft";
    registers[tmpLeft->name] = tmpLeft;
    auto tmpRight = std::make_shared<PseudoRegister>();
    tmpRight->name = "TMPright";
    registers[tmpRight->name] = tmpRight;
}

void generatePseudoCode(){
    for(auto& b: tree.getRoots()){
        generate(b.get());
    }
    _PUSH(HALT, "null");
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
    auto jElse = pushJump(JUMP);
    _WAIT_JUMP(jump, k);
    for(unsigned long i=cmd->getFirstElseIndex(); i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }

    _WAIT_JUMP(jElse, k);
}
void generateWhile(Command *cmd){
    uint64 startK = k;
    generate(cmd->getNested()[0].get());
    auto jump = code[k-1];
    for(unsigned long i=1; i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }
    auto jWhile = pushJump(JUMP);
    jWhile->setJumpReference(code[startK]);
    _WAIT_JUMP(jump, k);
}
void generateDoWhile(Command *cmd){
    auto jDoWhile = pushJump(JUMP);
    uint64 startK = k;
    generate(cmd->getNested()[0].get());
    auto jump = code[k-1];
    _WAIT_JUMP(jDoWhile, k);
    for(unsigned long i=1; i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }
    auto jWhile = pushJump(JUMP);
    jWhile->setJumpReference(code[startK]);
    _WAIT_JUMP(jump, k);
}
void generateFor(ForLoop *fl){
    auto it = fl->getIterator()->getName();
    auto from = getValueName(fl->getFrom()), to = getValueName(fl->getTo());

    auto pseudo = std::make_shared<PseudoRegister>();
    pseudo->isIterator = true;
    pseudo->name = it;
    registers[it] = pseudo;

    auto pseudoEnd = std::make_shared<PseudoRegister>();
    pseudoEnd->name = it + "END";
    registers[pseudoEnd->name] = pseudoEnd;
    generatePseudoAsmByCallType(to, LOAD);
    _PUSH(STORE, pseudoEnd->name)

    generatePseudoAsmByCallType(from, LOAD);
    auto condJump = pushJump(JUMP);
    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }
    _PUSH(LOAD, it)
    _PUSH(INC, "null")
    _WAIT_JUMP(condJump, k);
    _PUSH(STORE, it)
    _PUSH(SUB, pseudoEnd->name);
    _PUSH(DEC, "null");
    auto jFor = pushJump(JNEG);
    jFor->setJumpReference(code[startK]);
}
void generateForDown(ForLoop *fl){
    auto it = fl->getIterator()->getName();
    auto from = getValueName(fl->getFrom()), to = getValueName(fl->getTo());

    auto pseudo = std::make_shared<PseudoRegister>();
    pseudo->isIterator = true;
    pseudo->name = it;
    registers[it] = pseudo;
    auto pseudoEnd = std::make_shared<PseudoRegister>();
    pseudoEnd->name = it + "END";
    registers[pseudoEnd->name] = pseudoEnd;
    generatePseudoAsmByCallType(to, LOAD);
    _PUSH(STORE, pseudoEnd->name)

    generatePseudoAsmByCallType(from, LOAD);
    auto condJump = pushJump(JUMP);
    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }
    _PUSH(LOAD,it)

    _PUSH(DEC, "null");
    _WAIT_JUMP(condJump, k)
    _PUSH(STORE,it)
    _PUSH(SUB, pseudoEnd->name)
    _PUSH(INC, "null");

    auto jFor = pushJump(JPOS);
    jFor->setJumpReference(code[startK]);
}
void generateRead(Command *cmd){
    _PUSH(GET, "null");
    auto call = getCallName(*(cmd->getCalls()[0].get()));
    
    generatePseudoAsmByCallType(call, STORE);
}
void generateWrite(Command *cmd){
    auto val = cmd->getValue();
    auto call = getValueName(val);
    generatePseudoAsmByCallType(call, LOAD);

    _PUSH(PUT, "null");
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
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    _PUSH(SUB, "ACC")
    _PUSH(STORE, "TMP1exp")
    _PUSH(STORE, "TMP2exp")
    generatePseudoAsmByCallType(left, LOAD);
    _PUSH(STORE, "TMPleft")
    generatePseudoAsmByCallType(right, LOAD);
    _PUSH(STORE, "TMPright")
    _PUSH(STORE, "TMP3exp")
    auto jSign = pushJump(JNEG);
    _WAIT_JUMP(jSign, k+1)
    auto jFalse = pushJump(JUMP);
    _WAIT_JUMP(jFalse, k+2)
    _PUSH(SUB, "TMP3exp")
    _PUSH(SUB, "TMP3exp")
    uint64 startk = k;
    _PUSH(STORE, "TMP3exp")
    auto jWhile = pushJump(JZERO);
    _PUSH(SHIFT, "-1")
    _PUSH(SHIFT, "1")
    _PUSH(SUB, "TMP3exp")
    auto jMod = pushJump(JZERO);
    _PUSH(LOAD, "TMPleft")
    _PUSH(SHIFT, "TMP2exp")
    _PUSH(ADD, "TMP1exp")
    _PUSH(STORE, "TMP1exp")
    _WAIT_JUMP(jMod, k);
    _PUSH(LOAD, "TMP2exp")
    _PUSH(INC, "null")
    _PUSH(STORE, "TMP2exp")
    _PUSH(LOAD, "TMP3exp")
    _PUSH(SHIFT, "-1")
    auto jump = pushJump(JUMP);
    jump->setJumpReference(code[startk]);
    _WAIT_JUMP(jWhile, k);
    _PUSH(LOAD, "TMPright")
    auto jResult = pushJump(JNEG);
    _WAIT_JUMP(jResult, k+2)
    _PUSH(LOAD, "TMP1exp")
    auto jEnd = pushJump(JUMP);
    _WAIT_JUMP(jEnd, k+3)
    _PUSH(LOAD, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
}
void generateDiv(Expression *exp){
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    _PUSH(SUB, "ACC")
    _PUSH(STORE, "TMP1exp")
    generatePseudoAsmByCallType(right, LOAD);
    auto jBzero = pushJump(JZERO); //jump end
    _PUSH(STORE, "TMPright")
    _PUSH(STORE, "TMP2exp")
    auto jneg = pushJump(JNEG);
    _WAIT_JUMP(jneg, k+1)
    auto jumpStart = pushJump(JUMP);
    _WAIT_JUMP(jumpStart, k+3)
    _PUSH(SUB, "TMP2exp")
    _PUSH(SUB, "TMP2exp")
    _PUSH(STORE, "TMP2exp")
    generatePseudoAsmByCallType(left, LOAD);
    _PUSH(STORE, "TMPleft") 
    _PUSH(STORE, "TMP3exp")
    auto jneg1 = pushJump(JNEG);
    _WAIT_JUMP(jneg1, k+1)
    auto jump = pushJump(JUMP);
    _WAIT_JUMP(jump, k+3)
    _PUSH(SUB, "TMP3exp")
    _PUSH(SUB, "TMP3exp")
    uint64 startWhile = k;
    _PUSH(STORE, "TMP3exp")
    _PUSH(SUB, "TMP2exp")
    auto jEndWhile = pushJump(JNEG); //jump end bigger while
    //_WAIT_JUMP(jEndWhile, k+24)
    _PUSH(LOAD, "TMP2exp")
    _PUSH(STORE, "TMP4exp")
    _PUSH(SUB, "ACC")
    _PUSH(INC, "null")
    uint64 startInteriorWhile = k;
    _PUSH(STORE, "TMP6exp")
    _PUSH(LOAD, "TMP4exp")
    _PUSH(SUB, "TMP3exp")
    auto jEndInt = pushJump(JPOS); //jump end interior while
    //_WAIT_JUMP(jEndInt, k+6)
    _PUSH(LOAD, "TMP4exp")
    _PUSH(SHIFT, "1")
    _PUSH(STORE, "TMP4exp")
    _PUSH(LOAD, "TMP6exp")
    _PUSH(SHIFT, "1")
    auto jump1 = pushJump(JUMP);
    jump1 -> setJumpReference(code[startInteriorWhile]);
    _WAIT_JUMP(jEndInt, k)
    _PUSH(LOAD, "TMP6exp") //end interior while
    _PUSH(SHIFT, "-1")
    _PUSH(ADD, "TMP1exp")
    _PUSH(STORE, "TMP1exp")
    _PUSH(LOAD, "TMP4exp")
    _PUSH(SHIFT, "-1")
    _PUSH(STORE, "TMP5exp")
    _PUSH(LOAD, "TMP3exp")
    _PUSH(SUB, "TMP5exp")
    auto jump2 = pushJump(JUMP);
    jump2 -> setJumpReference(code[startWhile]);
    _WAIT_JUMP(jEndWhile, k)
    _PUSH(LOAD, "TMPright") //end bigger while
    auto jneg2 = pushJump(JNEG); //jump to next sign check
    _PUSH(LOAD, "TMPleft")
    auto jneg3 = pushJump(JNEG);
    _WAIT_JUMP(jneg3, k+2) 
    uint64 startFirstCheck = k;
    _PUSH(LOAD, "TMP1exp")
    auto jumpEnd = pushJump(JUMP); //jump end
    uint64 startSecondCheck = k;
    _PUSH(LOAD, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
    _PUSH(STORE, "TMP1exp")
    auto jumpCheck = pushJump(JUMP); //jump to end sign check
    _WAIT_JUMP(jneg2, k)    
    _PUSH(LOAD, "TMPleft")  //next sign check
    auto jneg4 = pushJump(JNEG);
    jneg4 ->setJumpReference(code[startFirstCheck]);
    auto jump3 = pushJump(JUMP);
    jump3->setJumpReference(code[startSecondCheck]);
    _WAIT_JUMP(jumpCheck, k);
    _PUSH(LOAD, "TMP3exp")  //end sign check
    auto jzero = pushJump(JZERO);
    _PUSH(LOAD, "TMP1exp")
    _PUSH(SUB, "1")
    auto jumpResult = pushJump(JUMP);
    _WAIT_JUMP(jzero, k)
    _PUSH(LOAD, "TMP1exp")
    
    _WAIT_JUMP(jumpResult, k)
    _WAIT_JUMP(jBzero, k)
    _WAIT_JUMP(jumpEnd, k)
    //end
}
void generateMod(Expression *exp){
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    generatePseudoAsmByCallType(right, LOAD);
    auto jzero = pushJump(JZERO); //jump end
    _PUSH(STORE, "TMPright")
    auto jneg = pushJump(JNEG);
    _WAIT_JUMP(jneg, k+1);
    auto jump = pushJump(JUMP);
    _WAIT_JUMP(jump, k+2)
    _PUSH(SUB, "TMPright")
    _PUSH(SUB, "TMPright")
    _PUSH(STORE, "TMP1exp")
    generatePseudoAsmByCallType(left, LOAD);
    auto jzero1 = pushJump(JZERO); //jump end
    _PUSH(STORE, "TMPleft")
    auto jneg1 = pushJump(JNEG);
    _WAIT_JUMP(jneg1, k+1)
    auto jump1 = pushJump(JUMP);
    _WAIT_JUMP(jump1, k+2)
    _PUSH(SUB, "TMPleft")
    _PUSH(SUB, "TMPleft")
    uint64 startBigWhile = k;
    _PUSH(STORE, "TMP2exp")
    _PUSH(SUB, "TMP1exp")
    auto jneg2 = pushJump(JNEG); //jump end big while
    _PUSH(LOAD, "TMP1exp")
    uint64 startInteriorWhile = k;
    _PUSH(STORE, "TMP3exp")
    _PUSH(SUB, "TMP2exp")
    auto jpos1 = pushJump(JPOS); //jump end interior while
    _PUSH(LOAD, "TMP3exp")
    _PUSH(SHIFT, "1")
    auto jump2 = pushJump(JUMP);
    jump2->setJumpReference(code[startInteriorWhile]);
    _WAIT_JUMP(jpos1, k);
    _PUSH(LOAD, "TMP3exp") //end interior while
    _PUSH(SHIFT, "-1")
    _PUSH(STORE, "TMP4exp")
    _PUSH(LOAD, "TMP2exp")
    _PUSH(SUB, "TMP4exp")
    auto jump3 = pushJump(JUMP);
    jump3->setJumpReference(code[startBigWhile]);
    _WAIT_JUMP(jneg2, k)
    _PUSH(LOAD, "TMP2exp") //end big while
    auto jzero2 = pushJump(JZERO); //jump end
    _PUSH(LOAD, "TMPleft") 
    auto jneg3 = pushJump(JNEG);
    _WAIT_JUMP(jneg3, k+7)
    _PUSH(LOAD, "TMPright")     
    auto jneg4 = pushJump(JNEG);
    _WAIT_JUMP(jneg4, k+2)
    _PUSH(LOAD, "TMP2exp")      //left > 0 && right > 0
    auto jump4 = pushJump(JUMP); //jump end
    _PUSH(LOAD, "TMP2exp")      //left > 0 && right < 0
    _PUSH(ADD, "TMPright")
    auto jump5 = pushJump(JUMP); //jump end
    _PUSH(LOAD, "TMPright")
    auto jneg5 = pushJump(JNEG);
    _WAIT_JUMP(jneg5, k+2) 
    _PUSH(SUB, "TMP2exp")       //left < 0 && right > 0
    auto jump6 = pushJump(JUMP); //jump end
    _PUSH(SUB, "ACC")           //left < 0 && right < 0
    _PUSH(SUB, "TMP2exp")

    _WAIT_JUMP(jump6, k)
    _WAIT_JUMP(jump5, k)
    _WAIT_JUMP(jump4, k)
    _WAIT_JUMP(jzero, k)
    _WAIT_JUMP(jzero1, k)
    _WAIT_JUMP(jzero2, k)
    //end
}

void generateCond(Condition *cond){
    if(cond->isResultExist()){
        bool result = cond->getResult();
        if(result){
            auto jResult = pushJump(JUMP);
            _WAIT_JUMP(jResult, k+1);
        }
        _PUSH(JUMP, "null");
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
    auto jtrue = pushJump(JZERO);
    _WAIT_JUMP(jtrue, k+1);
    _PUSH(JUMP, "null");
}
void generateNotEqual(Condition *cond){
    _PUSH(JZERO, "null");
}
void generateLesser(Condition *cond){
    auto jtrue = pushJump(JNEG);
    _WAIT_JUMP(jtrue, k+1);
    _PUSH(JUMP, "null");
}
void generateGreater(Condition *cond){
    auto jtrue = pushJump(JPOS);
    _WAIT_JUMP(jtrue, k+1);
    _PUSH(JUMP, "null");
}
void generateLesserEqual(Condition *cond){
    _PUSH(JPOS, "null");
}
void generateGreaterEqual(Condition *cond){
    _PUSH(JNEG, "null");
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
    _PUSH(STORE, call);
}
void generatePseudoLoad(std::string call){
    _PUSH(LOAD, call);
}
void generatePseudoSub(std::string call){
    _PUSH(SUB, call)
}
void generatePseudoAdd(std::string call){
    _PUSH(ADD, call)
}
void generatePseudoShift(std::string call){
    _PUSH(SHIFT, call)
}

void generatePseudoStorei(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(STORE, "TMParr")
    _PUSH(LOAD, idx)
    _PUSH(ADD, arr+"-off")
    _PUSH(STORE, "TMP2arr")
    _PUSH(LOAD, "TMParr")
    _PUSH(STOREI, "TMP2arr")  
}
void generatePseudoLoadi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(LOAD, idx)
    _PUSH(ADD, arr+"-off")
    _PUSH(LOADI, "ACC")   
}
void generatePseudoSubi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(STORE, "TMParr")
    _PUSH(LOAD, idx)
    _PUSH(ADD, arr+"-off")
    _PUSH(LOADI, "ACC")
    _PUSH(STORE, "TMP2arr")
    _PUSH(LOAD, "TMParr")
    _PUSH(SUB, "TMP2arr")
}
void generatePseudoAddi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(STORE, "TMParr")
    _PUSH(LOAD, idx)
    _PUSH(ADD, arr+"-off")
    _PUSH(LOADI, "ACC")
    _PUSH(ADD, "TMParr")
}
void generatePseudoShifti(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(STORE, "TMParr")
    _PUSH(LOAD, idx)
    _PUSH(ADD, arr+"-off")
    _PUSH(LOADI, "ACC")
    _PUSH(STORE, "TMP2arr")
    _PUSH(LOAD, "TMParr")
    _PUSH(SHIFT, "TMP2arr")
}

std::shared_ptr<PseudoAsm> pushJump(Instruction type){
    auto jump = std::make_shared<PseudoAsm>(k, type, "null");
    _PUSH_PSEUDO(jump);
    return jump;
}