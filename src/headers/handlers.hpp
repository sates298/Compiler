#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "tree.hpp"
#include "errors.hpp"

void handleVariableDeclaration(std::string name);
void handleArrayDeclaration(std::string name, long long first, long long last);


#endif