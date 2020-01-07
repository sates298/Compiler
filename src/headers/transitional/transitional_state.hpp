#ifndef TRANS_HPP
#define TRANS_HPP

#include "../assembler/assembler.hpp"
#include "../global.hpp"

// extern uint64 k;

class PseudoAsm{
    private:
        uint64 index;
        Instruction instr;
        std::string argument;
        bool inJump;
    public:
    PseudoAsm(uint64 k, Instruction i, std::string argument, bool jump);
    virtual ~PseudoAsm() = default;
    static void shiftCode(uint64 k, std::vector<std::shared_ptr<PseudoAsm>> code);
    uint64 getIndex();
    void shiftIndex(uint64 s);
    bool isInJump();
    Instruction getInstr();
    std::string getArgument();
};

struct PseudoRegister{
    std::string name;
    uint64 index;
};

extern std::map<std::string, PseudoRegister> registers;
extern std::vector<std::shared_ptr<PseudoAsm>> code; 

#endif