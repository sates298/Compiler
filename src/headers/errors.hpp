#ifndef ERRORS_HPP
#define ERRORS_HPP
#include<iostream>
#include<string>

std::string red = "\033[0;31m";
std::string violet = "\033[0;35m";
std::string norm = "\033[0m";

void error(std::string msg, int line_no);
void error(std::string msg);
void warning(std::string msg, int line_no);
void warning(std::string msg);
#endif