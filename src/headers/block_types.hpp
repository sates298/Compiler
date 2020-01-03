#ifndef BLOCKS_T_HPP
#define BLOCKS_T_HPP

enum BlockType{
    CMD, EXPR, COND
};

enum CommandType{
    CASSIGN, CIF, CIFELSE, CWHILE, CDOWHILE,
    CFOR, CFORDOWN, CREAD, CWRITE 
};

enum ExpressionType{
    EPLUS, EMINUS, ETIMES, EDIV, EMOD, ENULL
};

enum ConditionType{
    CEQ, CNEQ, CLE, CGE, CLEQ, CGEQ
};

#endif