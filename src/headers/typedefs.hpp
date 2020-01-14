#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <iostream>

typedef long long int64;
typedef unsigned long long uint64;

extern bool optimization;
extern bool verbose;
extern bool debug;

#define log(msg) { if(debug) {std::clog<< msg <<'\n';}}
#endif