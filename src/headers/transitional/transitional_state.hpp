#ifndef TRANS_HPP
#define TRANS_HPP

#include "../assembler/assembler.hpp"
#include "../AST/tree.hpp"
#include<map>
#include<deque>

class PseudoAsm{
    private:
        uint64 index;
        Instruction instr;
        std::string argument;
        std::shared_ptr<PseudoAsm> jumpReference;
        bool jumped;
    public:
    PseudoAsm(uint64 k, Instruction i, std::string argument);
    virtual ~PseudoAsm() = default;
    static void shiftCode(uint64 from, int64 how, std::deque<std::shared_ptr<PseudoAsm>> code);
    uint64 getIndex();
    void shiftIndex(int64 s);
    void setJumpReference(std::shared_ptr<PseudoAsm> reference);
    std::shared_ptr<PseudoAsm> getJumpReference();
    Instruction getInstr();
    std::string getArgument();
    void setJumped(bool b);
    bool isJumped();

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

typedef std::deque<std::shared_ptr<PseudoAsm>> pseudoQue;

extern std::map<std::string, std::shared_ptr<PseudoRegister>> registers;
extern pseudoQue code;
extern std::map<uint64, pseudoQue> waitingJumps;

void generatePseudoCode();
void generatePseudoRegisters();
void valid();
void optimizeCode();

#endif