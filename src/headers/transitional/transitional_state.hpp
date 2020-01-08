#ifndef TRANS_HPP
#define TRANS_HPP

#include "../assembler/assembler.hpp"
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
};

typedef std::vector<std::shared_ptr<PseudoAsm>> pseudoVec;

extern std::map<std::string, PseudoRegister> registers;
extern pseudoVec code;
extern std::map<uint64, pseudoVec> waitingJumps;

void generatePseudoCode();
void valid();


#endif