#include "../headers/errors.hpp"

void error(std::string msg, int line_no, bool fatal){
    std::cerr << red << "[ERROR] " << msg << " in line "<< line_no << norm << "\n";
}

void error(std::string msg, bool fatal){
    std::cerr << red << "[ERROR] " << msg << norm << "\n";
}

void error(std::string msg, std::string sol, bool fatal){
    std::cerr << red << "[ERROR] " << msg << norm << "\n";
    std::cerr << green << "[FIX]" << sol << norm << "\n";
}

void error(std::string msg, std::string sol,int line_no, bool fatal){
    std::cerr << red << "[ERROR] " << msg << " in line "<< line_no << norm << "\n";
    std::cerr << green << "[FIX]" << sol << norm << "\n";
}

void warning(std::string msg, int line_no){
    std::cerr << magenta << "[WARNING] " << msg << " in line "<< line_no << norm << "\n";
}

void warning(std::string msg){
    std::cerr << magenta << "[WARNING] " << msg << norm << "\n";
}