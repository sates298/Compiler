#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "tree.hpp"
#include <iostream>

#define log(msg) {std::cerr<< msg <<'\n';}

extern int yylineno;

extern Tree tree;
extern std::map<unsigned long long, long long> registers;


#endif