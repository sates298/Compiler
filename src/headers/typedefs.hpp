#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <iostream>

#define log(msg) {std::clog<< msg <<'\n';}

typedef long long int64;
typedef unsigned long long uint64;

extern bool optimization;
extern bool debug;
#endif