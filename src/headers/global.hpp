#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "tree.hpp"

extern int yylineno;

extern Tree tree;
extern std::map<uint64, int64> registers;

extern bool optimization;

#endif