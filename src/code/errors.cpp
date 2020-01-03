#include "../headers/errors.hpp"

void error(std::string msg, int line_no){
    std::cerr << red << "[ERROR] " << msg << " in line "<< line_no << norm << "\n";
}

void error(std::string msg){
    std::cerr << red << "[ERROR] " << msg << norm << "\n";
}

void warning(std::string msg, int line_no){
    std::cerr << violet << "[WARNING] " << msg << " in line "<< line_no << norm << "\n";
}

void warning(std::string msg){
    std::cerr << violet << "[WARNING] " << msg << norm << "\n";
}