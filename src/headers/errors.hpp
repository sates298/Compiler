#ifndef ERRORS_HPP
#define ERRORS_HPP
#include <iostream>
#include <string>
#include <stdlib.h>

const std::string red = "\033[0;31m";
const std::string magenta = "\033[0;35m";
const std::string green = "\033[0;32m";
const std::string cyan = "\033[0;36m";
const std::string norm = "\033[0m";

extern bool errorAttempts;

void error(std::string msg, int line_no, bool fatal);
void error(std::string msg, bool fatal);
void error(std::string msg, std::string sol,int line_no, bool fatal);
void error(std::string msg, std::string sol, bool fatal);

void warning(std::string msg, int line_no);
void warning(std::string msg);

extern bool verbose;
#endif