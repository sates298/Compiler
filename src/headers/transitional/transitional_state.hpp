#ifndef TRANS_HPP
#define TRANS_HPP

#include "../assembler/assembler.hpp"
#include "../AST/tree.hpp"
#include<map>

class PseudoAsm{
    private:
        uint64 index;
        Instruction instr;
        std::string argument;
        std::shared_ptr<PseudoAsm> jumpReference;
    public:
    PseudoAsm(uint64 k, Instruction i, std::string argument);
    virtual ~PseudoAsm() = default;
    static void shiftCode(uint64 k, std::vector<std::shared_ptr<PseudoAsm>> code);
    uint64 getIndex();
    void shiftIndex(uint64 s);
    void setJumpReference(std::shared_ptr<PseudoAsm> reference);
    std::shared_ptr<PseudoAsm> getJumpReference();
    Instruction getInstr();
    std::string getArgument();

    std::string toString();
};

struct PseudoRegister{
    std::string name;
    uint64 index;
    bool isNumber;
    bool isOffset;
    int64 offsetVal;
    bool isArray;
    int64 sizeArr;
    bool isIterator;
    Variable *var = nullptr;

    std::string toString();
};

typedef std::vector<std::shared_ptr<PseudoAsm>> pseudoVec;

extern std::map<std::string, std::shared_ptr<PseudoRegister>> registers;
extern pseudoVec code;
extern std::map<uint64, pseudoVec> waitingJumps;

void generatePseudoCode();
void generatePseudoRegisters();
void valid();
void optimizeCode();


#endif