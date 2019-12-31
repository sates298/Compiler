#include "../headers/errors.hpp"

void error(const char* msg, int line_no){
    std::cerr << "\033[0;31m" << "[ERROR] " << msg << " in line "<< line_no << "\033[0m \n";
}

void warning(const char* msg, int line_no){
    std::cerr << "\033[0;35m" << "[WARNING] " << msg << " in line "<< line_no << "\033[0m \n";
}
