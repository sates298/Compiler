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
    auto jElse = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH_PSEUDO(jElse);
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
    auto jWhile = std::make_shared<PseudoAsm>(k, JUMP, "null");
    jWhile->setJumpReference(code[startK]);
    _PUSH_PSEUDO(jWhile);
    _WAIT_JUMP(jump, k);
}
void generateDoWhile(Command *cmd){
    auto jDoWhile = std::make_shared<PseudoAsm>(k, JUMP, "null");
    _PUSH_PSEUDO(jDoWhile);
    uint64 startK = k;
    generate(cmd->getNested()[0].get());
    auto jump = code[k-1];
    _WAIT_JUMP(jDoWhile, k);
    for(unsigned long i=1; i<cmd->getNested().size(); i++){
        generate(cmd->getNested()[i].get());
    }
    auto jWhile = std::make_shared<PseudoAsm>(k, JUMP, "null");
    jWhile->setJumpReference(code[startK]);
    _PUSH_PSEUDO(jWhile);
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
    _PUSH_PSEUDO(condJump);
    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }
    generatePseudoAsmByCallType(it, LOAD);

    _PUSH(INC, "null");

    _WAIT_JUMP(condJump, k);

    generatePseudoAsmByCallType(it, STORE); //it'll be always normal store - this is iterator
    generatePseudoAsmByCallType(to, SUB);
    _PUSH(DEC, "null");
    auto jFor = std::make_shared<PseudoAsm>(k, JNEG, "null");
    jFor->setJumpReference(code[startK]);
    _PUSH_PSEUDO(jFor);

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
    _PUSH_PSEUDO(condJump);

    uint64 startK = k;
    for(auto& n:fl->getNested()){
        generate(n.get());
    }
    generatePseudoAsmByCallType(it, LOAD);

    _PUSH(DEC, "null");
    _WAIT_JUMP(condJump, k);
    generatePseudoAsmByCallType(it, STORE); //it'll be always normal store - this is iterator
    generatePseudoAsmByCallType(to, SUB);
    _PUSH(INC, "null");

    auto jFor = std::make_shared<PseudoAsm>(k, JPOS, "null");
    jFor->setJumpReference(code[startK]);
    _PUSH_PSEUDO(jFor);
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
    auto jSign = std::make_shared<PseudoAsm>(k,JNEG,"null");
    _WAIT_JUMP(jSign, k+2)
    _PUSH_PSEUDO(jSign)
    auto jFalse = std::make_shared<PseudoAsm>(k,JUMP,"null");
    _WAIT_JUMP(jFalse, k+3)
    _PUSH_PSEUDO(jFalse)
    _PUSH(SUB, "TMP3exp")
    _PUSH(SUB, "TMP3exp")
    uint64 startk = k;
    _PUSH(STORE, "TMP3exp")
    auto jWhile = std::make_shared<PseudoAsm>(k,JZERO,"null");
    _PUSH_PSEUDO(jWhile)
    _PUSH(SHIFT, "-1")
    _PUSH(SHIFT, "1")
    _PUSH(SUB, "TMP3exp")
    auto jMod = std::make_shared<PseudoAsm>(k,JZERO,"null");
    _PUSH_PSEUDO(jMod)
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
    auto jump = std::make_shared<PseudoAsm>(k,JUMP,"null");
    jump->setJumpReference(code[startk]);
    _PUSH_PSEUDO(jump)
    _WAIT_JUMP(jWhile, k);
    _PUSH(LOAD, "TMPright")
    auto jResult = std::make_shared<PseudoAsm>(k,JNEG,"null");
    _WAIT_JUMP(jResult, k+3)
    _PUSH_PSEUDO(jResult)
    _PUSH(LOAD, "TMP1exp")
    auto jEnd = std::make_shared<PseudoAsm>(k,JUMP,"null");
    _WAIT_JUMP(jEnd, k+4)
    _PUSH_PSEUDO(jEnd)
    _PUSH(LOAD, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
}
void generateDiv(Expression *exp){
    auto left = getValueName(exp->getLeft()), right = getValueName(exp->getRight());
    _PUSH(SUB, "ACC")
    _PUSH(STORE, "TMP1exp")
    generatePseudoAsmByCallType(right, LOAD);
    auto jBzero = std::make_shared<PseudoAsm>(k,JZERO,"null"); //jump end
    _PUSH_PSEUDO(jBzero)
    _PUSH(STORE, "TMPright")
    _PUSH(STORE, "TMP2exp")
    auto jneg = std::make_shared<PseudoAsm>(k,JNEG,"null");
    _WAIT_JUMP(jneg, k+2)
    _PUSH_PSEUDO(jneg)
    auto jumpStart = std::make_shared<PseudoAsm>(k,JUMP,"null");
    _WAIT_JUMP(jumpStart, k+4)
    _PUSH_PSEUDO(jumpStart)
    _PUSH(SUB, "TMP2exp")
    _PUSH(SUB, "TMP2exp")
    _PUSH(STORE, "TMP2exp")
    generatePseudoAsmByCallType(left, LOAD);
    _PUSH(STORE, "TMPleft") 
    _PUSH(STORE, "TMP3exp")
    auto jneg1 = std::make_shared<PseudoAsm>(k,JNEG,"null");
    _WAIT_JUMP(jneg1, k+2)
    _PUSH_PSEUDO(jneg1)
    auto jump = std::make_shared<PseudoAsm>(k,JUMP,"null");
    _WAIT_JUMP(jump, k+4)
    _PUSH_PSEUDO(jump)
    _PUSH(SUB, "TMP3exp")
    _PUSH(SUB, "TMP3exp")
    uint64 startWhile = k;
    _PUSH(STORE, "TMP3exp")
    _PUSH(SUB, "TMP2exp")
    auto jEndWhile = std::make_shared<PseudoAsm>(k,JNEG,"null"); //jump end bigger while
    //_WAIT_JUMP(jEndWhile, k+25)
    _PUSH_PSEUDO(jEndWhile)
    _PUSH(LOAD, "TMP2exp")
    _PUSH(STORE, "TMP4exp")
    _PUSH(SUB, "ACC")
    _PUSH(INC, "null")
    uint64 startInteriorWhile = k;
    _PUSH(STORE, "TMP6exp")
    _PUSH(LOAD, "TMP4exp")
    _PUSH(SUB, "TMP3exp")
    auto jEndInt = std::make_shared<PseudoAsm>(k,JPOS,"null"); //jump end interior while
    //_WAIT_JUMP(jEndInt, k+7)
    _PUSH_PSEUDO(jEndInt)
    _PUSH(LOAD, "TMP4exp")
    _PUSH(SHIFT, "1")
    _PUSH(STORE, "TMP4exp")
    _PUSH(LOAD, "TMP6exp")
    _PUSH(SHIFT, "1")
    auto jump1 = std::make_shared<PseudoAsm>(k,JUMP,"null");
    jump1 -> setJumpReference(code[startInteriorWhile]);
    _PUSH_PSEUDO(jump1) //last instruction of interior while
    _WAIT_JUMP(jEndInt, k)
    _PUSH(LOAD, "TMP6exp")
    _PUSH(SHIFT, "-1")
    _PUSH(ADD, "TMP1exp")
    _PUSH(STORE, "TMP1exp")
    _PUSH(LOAD, "TMP4exp")
    _PUSH(SHIFT, "-1")
    _PUSH(STORE, "TMP5exp")
    _PUSH(LOAD, "TMP3exp")
    _PUSH(SUB, "TMP5exp")
    auto jump2 = std::make_shared<PseudoAsm>(k,JUMP,"null");
    jump2 -> setJumpReference(code[startWhile]);
    _PUSH_PSEUDO(jump2) //last instruction of bigger while
    _WAIT_JUMP(jEndWhile, k)
    _PUSH(LOAD, "TMPright")
    auto jneg2 = std::make_shared<PseudoAsm>(k,JNEG,"null"); //jump to next sign check
    _PUSH_PSEUDO(jneg2)
    _PUSH(LOAD, "TMPleft")
    auto jneg3 = std::make_shared<PseudoAsm>(k,JNEG,"null");
    _WAIT_JUMP(jneg3, k+3) 
    _PUSH_PSEUDO(jneg3)
    uint64 startFirstCheck = k;
    _PUSH(LOAD, "TMP1exp")
    auto jumpEnd = std::make_shared<PseudoAsm>(k,JUMP,"null"); //jump end
    _PUSH_PSEUDO(jumpEnd)
    uint64 startSecondCheck = k;
    _PUSH(LOAD, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
    _PUSH(SUB, "TMP1exp")
    _PUSH(STORE, "TMP1exp")
    auto jumpCheck = std::make_shared<PseudoAsm>(k,JUMP,"null"); //jump to end sign check
    _PUSH_PSEUDO(jumpCheck)
    _WAIT_JUMP(jneg2, k)    //next sign check
    _PUSH(LOAD, "TMPleft")
    auto jneg4 = std::make_shared<PseudoAsm>(k,JNEG,"null");
    jneg4 ->setJumpReference(code[startFirstCheck]);
    _PUSH_PSEUDO(jneg4)
    auto jump3 = std::make_shared<PseudoAsm>(k,JUMP,"null");
    jump3->setJumpReference(code[startSecondCheck]);
    _PUSH_PSEUDO(jump3) //last instruction in sign check

    _WAIT_JUMP(jumpCheck, k);
    //check if is needed -1 to result
    _PUSH(LOAD, "TMP3exp")
    
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
        } //todo zrobić, żeby pseudoAsm wiedział, że wskazuje na niego jump
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
    auto jtrue = std::make_shared<PseudoAsm>(k, JZERO, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH_PSEUDO(jtrue);
    _PUSH(JUMP, "null");
}
void generateNotEqual(Condition *cond){
    _PUSH(JZERO, "null");
}
void generateLesser(Condition *cond){
    auto jtrue = std::make_shared<PseudoAsm>(k, JNEG, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH_PSEUDO(jtrue);
    _PUSH(JUMP, "null");
}
void generateGreater(Condition *cond){
    auto jtrue = std::make_shared<PseudoAsm>(k, JPOS, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH_PSEUDO(jtrue);
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