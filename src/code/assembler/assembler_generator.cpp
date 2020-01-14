#include "../../headers/assembler/assembler_generator.hpp"

uint64 addr = 0;
uint64 regIdx = 1; //0 - accumulator
asmVec finalCode;

auto reset = std::make_shared<Asm>(SUB, 0);
auto inc = std::make_shared<Asm>(INC, 0);
auto dec = std::make_shared<Asm>(DEC, 0);
auto get = std::make_shared<Asm>(GET, 0);
auto put = std::make_shared<Asm>(PUT, 0);
auto halt = std::make_shared<Asm>(HALT, 0);

void generateRealRegisters(){
    for(auto& [k,r] : registers){
        if(k == "ACC"){
            continue;
        }
        if(!r->isArray){
            r->index = regIdx;
            regIdx++;
        }else{
            auto off = registers[r->name + "-off"];
            off->offsetVal += regIdx; 
            r->index = regIdx;
            regIdx += r->sizeArr;
        }
    }
}

void generateNumber(PseudoRegister *pseudo){
    int64 num;
    if(pseudo->isOffset){
        num = pseudo->offsetVal;
    }else{
        num = std::stoll(pseudo->name);
    }
    _PUSH_ASM(reset);
    int64 tmp = num;
    if(num < 0){
        tmp = -1*num;
    }
    auto shift = std::make_shared<PseudoAsm>(addr, SHIFT, "1");
    std::vector<char> generator;
    while(tmp){
        if(tmp&1){
            generator.emplace_back('i');
        }
        tmp >>= 1;
        if(tmp){  
            generator.emplace_back('s');
        }   
    }
    for(int i=generator.size()-1; i>=0; i--){
        if(generator[i] == 's'){
            generateShift(shift.get());
        }else{
            if(num < 0){
                _PUSH_ASM(dec);
            }else{
                _PUSH_ASM(inc);
            }
        }
    }
    auto reg = registers[pseudo->name];
    _PUSH_INS(STORE, reg->index)
}
void generateConstants(){
    // if(optimization){
    //     _PUSH_ASM(reset);
    //     smartGeneratingConstants();
    // }else{
        _PUSH_ASM(reset);
        _PUSH_ASM(inc);
        _PUSH_PSEUDO_INS(STORE, "1")
        _PUSH_ASM(dec);
        _PUSH_ASM(dec);
        _PUSH_PSEUDO_INS(STORE, "-1")
        _PUSH_ASM(inc);
        for(auto& [k,r]: registers){
            if(r->isNumber && (r->name != "1" && r->name != "-1" )){
                generateNumber(r.get());
            }
        }
    // }
}

void generateFromPseudoAsm(){
    generateConstants();
    PseudoAsm::shiftCode(0,addr,code);
    for(auto& ins: code){
        generateAsm(ins.get());
    }
}

void generateAsm(PseudoAsm *p){
    switch (p->getInstr()){
    case GET:
        _PUSH_ASM(get);
        break;
    case PUT:
        _PUSH_ASM(put);
        break;
    case LOAD:
        generateLoad(p);
        break;
    case STORE:
        generateStore(p);
        break;
    case LOADI:
        generateLoadi(p);
        break;
    case STOREI:
        generateStorei(p);
        break;
    case ADD:
        generateAdd(p);
        break;
    case SUB:
        generateSub(p);
        break;
    case SHIFT:
        generateShift(p);
        break;
    case INC:
        _PUSH_ASM(inc);
        break;
    case DEC:
        _PUSH_ASM(dec);
        break;
    case JUMP:
        generateJump(p);
        break;
    case JPOS:
        generateJpos(p);
        break;
    case JZERO:
        generateJzero(p);
        break;
    case JNEG:
        generateJneg(p);
        break;
    case HALT:
        _PUSH_ASM(halt);
        break;
    default:
        break;
    }
}

void generateStore(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    _PUSH_INS(STORE, reg->index)
}
void generateLoad(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    _PUSH_INS(LOAD, reg->index)
}

void generateStorei(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    _PUSH_INS(STOREI, reg->index)
}
void generateLoadi(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    _PUSH_INS(LOADI, reg->index)
}

void generateJump(PseudoAsm *p){
    _PUSH_INS(JUMP, p->getJumpReference()->getIndex())
}
void generateJzero(PseudoAsm *p){
    _PUSH_INS(JZERO, p->getJumpReference()->getIndex())
}
void generateJpos(PseudoAsm *p){
    _PUSH_INS(JPOS, p->getJumpReference()->getIndex())
}
void generateJneg(PseudoAsm *p){
    _PUSH_INS(JNEG, p->getJumpReference()->getIndex())
}
void generateShift(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    _PUSH_INS(SHIFT, reg->index)
}
void generateAdd(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    _PUSH_INS(ADD, reg->index)
}
void generateSub(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    _PUSH_INS(SUB, reg->index)
}