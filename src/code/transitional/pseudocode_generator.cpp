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
    for(int i=1; i<5; i++){
        auto tmpExp = std::make_shared<PseudoRegister>();
        tmpExp->name = "TMP"+std::to_string(i)+"exp";
        registers[tmpExp->name] = tmpExp;
    }
}

void generatePseudoCode(){
    for(auto& b: tree.getRoots()){
        generate(b.get());
    }
    _PUSH(k, HALT, "null");
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

    _PUSH(k, INC, "null");

    _WAIT_JUMP(condJump, k);

    generatePseudoAsmByCallType(it, STORE); //it'll be always normal store - this is iterator
    generatePseudoAsmByCallType(to, SUB);
    _PUSH(k, DEC, "null");
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

    _PUSH(k, DEC, "null");
    _WAIT_JUMP(condJump, k);
    generatePseudoAsmByCallType(it, STORE); //it'll be always normal store - this is iterator
    generatePseudoAsmByCallType(to, SUB);
    _PUSH(k, INC, "null");

    auto jFor = std::make_shared<PseudoAsm>(k, JPOS, "null");
    jFor->setJumpReference(code[startK]);
    _PUSH_PSEUDO(jFor);
}
void generateRead(Command *cmd){
    _PUSH(k, GET, "null");
    auto call = getCallName(*(cmd->getCalls()[0].get()));
    
    generatePseudoAsmByCallType(call, STORE);
}
void generateWrite(Command *cmd){
    auto val = cmd->getValue();
    auto call = getValueName(val);
    generatePseudoAsmByCallType(call, LOAD);

    _PUSH(k, PUT, "null");
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
    _PUSH(k, SUB, "ACC")
    _PUSH(k, STORE, "TMP1exp")
    _PUSH(k, STORE, "TMP2exp")
    generatePseudoAsmByCallType(right, LOAD);
    _PUSH(k, STORE, "TMP3exp")
    auto jSign = std::make_shared<PseudoAsm>(k,JNEG,"null");
    _WAIT_JUMP(jSign, k+2)
    _PUSH_PSEUDO(jSign)
    auto jFalse = std::make_shared<PseudoAsm>(k,JUMP,"null");
    _WAIT_JUMP(jFalse, k+3)
    _PUSH_PSEUDO(jFalse)
    _PUSH(k, SUB, "TMP3exp")
    _PUSH(k, SUB, "TMP3exp")
    // _PUT_DEBUG
    uint64 startk = k;
    _PUSH(k, STORE, "TMP3exp")
    auto jWhile = std::make_shared<PseudoAsm>(k,JZERO,"null");
    _PUSH_PSEUDO(jWhile)
    _PUSH(k, SHIFT, "-1")
    _PUSH(k, SHIFT, "1")
    _PUSH(k, SUB, "TMP3exp")
    // _PUT_DEBUG
    auto jMod = std::make_shared<PseudoAsm>(k,JZERO,"null");
    _PUSH_PSEUDO(jMod)
    generatePseudoAsmByCallType(left, LOAD);
    _PUSH(k, SHIFT, "TMP2exp")
    _PUSH(k, ADD, "TMP1exp")
    _PUSH(k, STORE, "TMP1exp")
    _WAIT_JUMP(jMod, k);
    _PUSH(k, LOAD, "TMP2exp")
    _PUSH(k, INC, "null")
    _PUSH(k, STORE, "TMP2exp")
    // _PUT_DEBUG
    _PUSH(k, LOAD, "TMP3exp")
    _PUSH(k, SHIFT, "-1")
    auto jump = std::make_shared<PseudoAsm>(k,JUMP,"null");
    jump->setJumpReference(code[startk]);
    _PUSH_PSEUDO(jump)
    _WAIT_JUMP(jWhile, k);
    generatePseudoAsmByCallType(right, LOAD);
    auto jResult = std::make_shared<PseudoAsm>(k,JNEG,"null");
    _WAIT_JUMP(jResult, k+3)
    _PUSH_PSEUDO(jResult)
    _PUSH(k, LOAD, "TMP1exp")
    auto jEnd = std::make_shared<PseudoAsm>(k,JUMP,"null");
    _WAIT_JUMP(jEnd, k+4)
    _PUSH_PSEUDO(jEnd)
    _PUSH(k, LOAD, "TMP1exp")
    _PUSH(k, SUB, "TMP1exp")
    _PUSH(k, SUB, "TMP1exp")
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
            _PUSH_PSEUDO(jResult);
        } //todo zrobić, żeby pseudoAsm wiedział, że wskazuje na niego jump
        _PUSH(k, JUMP, "null");
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
    _PUSH(k, JUMP, "null");
}
void generateNotEqual(Condition *cond){
    _PUSH(k, JZERO, "null");
}
void generateLesser(Condition *cond){
    auto jtrue = std::make_shared<PseudoAsm>(k, JNEG, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH_PSEUDO(jtrue);
    _PUSH(k, JUMP, "null");
}
void generateGreater(Condition *cond){
    auto jtrue = std::make_shared<PseudoAsm>(k, JPOS, "null");
    _WAIT_JUMP(jtrue, k+2);
    _PUSH_PSEUDO(jtrue);
    _PUSH(k, JUMP, "null");
}
void generateLesserEqual(Condition *cond){
    _PUSH(k, JPOS, "null");
}
void generateGreaterEqual(Condition *cond){
    _PUSH(k, JNEG, "null");
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
    _PUSH(k, STORE, call);
}
void generatePseudoLoad(std::string call){
    _PUSH(k, LOAD, call);
}
void generatePseudoSub(std::string call){
    _PUSH(k, SUB, call)
}
void generatePseudoAdd(std::string call){
    _PUSH(k, ADD, call)
}
void generatePseudoShift(std::string call){
    _PUSH(k, SHIFT, call)
}

void generatePseudoStorei(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(k, STORE, "TMParr")
    _PUSH(k, LOAD, idx)
    _PUSH(k, ADD, arr+"-off")
    _PUSH(k, STORE, "TMP2arr")
    _PUSH(k, LOAD, "TMParr")
    _PUSH(k, STOREI, "TMP2arr")  
}
void generatePseudoLoadi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(k, LOAD, idx)
    _PUSH(k, ADD, arr+"-off")
    _PUSH(k, LOADI, "ACC")   
}
void generatePseudoSubi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(k, STORE, "TMParr")
    _PUSH(k, LOAD, idx)
    _PUSH(k, ADD, arr+"-off")
    _PUSH(k, LOADI, "ACC")
    _PUSH(k, STORE, "TMP2arr")
    _PUSH(k, LOAD, "TMParr")
    _PUSH(k, SUB, "TMP2arr")
}
void generatePseudoAddi(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(k, STORE, "TMParr")
    _PUSH(k, LOAD, idx)
    _PUSH(k, ADD, arr+"-off")
    _PUSH(k, LOADI, "ACC")
    _PUSH(k, ADD, "TMParr")
}
void generatePseudoShifti(std::string call){
    auto it = call.find(":");
    auto arr = call.substr(0,it);
    auto idx = call.substr(it+1);
    _PUSH(k, STORE, "TMParr")
    _PUSH(k, LOAD, idx)
    _PUSH(k, ADD, arr+"-off")
    _PUSH(k, LOADI, "ACC")
    _PUSH(k, STORE, "TMP2arr")
    _PUSH(k, LOAD, "TMParr")
    _PUSH(k, SHIFT, "TMP2arr")
}
