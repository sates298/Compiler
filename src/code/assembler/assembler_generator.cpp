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

    auto store = std::make_shared<PseudoAsm>(addr, STORE, pseudo->name);
    generateStore(store.get());
}
void generateConstants(){
    _PUSH_ASM(reset);

    _PUSH_ASM(inc);
    auto store = std::make_shared<PseudoAsm>(addr, STORE, "1");
    generateStore(store.get());
    std::vector<std::shared_ptr<PseudoRegister>> numbers;
    for(auto& [k,r]: registers){
        if(r->isNumber && r->name != "1"){
            if(optimization){
                numbers.emplace_back(r);
            }else{
                generateNumber(r.get());
            }
        }
    }
    if(optimization){
        //todo sortowanie numbers
        for(auto n: numbers){
            //todo mądre generowanie numerów
        }
    }
}

void generateFromPseudoAsm(){
    generateConstants();
    PseudoAsm::shiftCode(addr,code);
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
    auto assm = std::make_shared<Asm>(STORE, reg->index);
    _PUSH_ASM(assm);
}
void generateLoad(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    auto assm = std::make_shared<Asm>(LOAD, reg->index);
    _PUSH_ASM(assm);
}

void generateStorei(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    auto assm = std::make_shared<Asm>(STOREI, reg->index);
    _PUSH_ASM(assm);
}
void generateLoadi(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    auto assm = std::make_shared<Asm>(LOADI, reg->index);
    _PUSH_ASM(assm);
}

void generateJump(PseudoAsm *p){
    auto assm = std::make_shared<Asm>(JUMP,p->getJumpReference()->getIndex());
    _PUSH_ASM(assm);
}
void generateJzero(PseudoAsm *p){
    auto assm = std::make_shared<Asm>(JZERO, p->getJumpReference()->getIndex());
    _PUSH_ASM(assm);
}
void generateJpos(PseudoAsm *p){
    auto assm = std::make_shared<Asm>(JPOS, p->getJumpReference()->getIndex());
    _PUSH_ASM(assm);
}
void generateJneg(PseudoAsm *p){
    auto assm = std::make_shared<Asm>(JNEG, p->getJumpReference()->getIndex());
    _PUSH_ASM(assm);
}
void generateShift(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    auto assm = std::make_shared<Asm>(SHIFT, reg->index);
    _PUSH_ASM(assm);
}
void generateAdd(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    auto assm = std::make_shared<Asm>(ADD, reg->index);
    _PUSH_ASM(assm);
}
void generateSub(PseudoAsm *p){
    auto reg = registers[p->getArgument()];
    auto assm = std::make_shared<Asm>(SUB, reg->index);
    _PUSH_ASM(assm);
}