#include "../headers/errors.hpp"

bool errorAttempts = false;

void error(std::string msg, int line_no, bool fatal){
    errorAttempts = true;
    std::cerr << red << "[ERROR] " << msg << " in block that ends in line "<< line_no << norm << "\n";
    if(fatal){
        std::exit(EXIT_FAILURE);
    }
}

void error(std::string msg, bool fatal){
    errorAttempts = true;
    std::cerr << red << "[ERROR] " << msg << norm << "\n";
    if(fatal){
        std::exit(EXIT_FAILURE);
    }
}

void error(std::string msg, std::string sol, bool fatal){
    errorAttempts = true;
    std::cerr << red << "[ERROR] " << msg << norm << "\n";
    std::cerr << green << "[FIX] " << sol << norm << "\n";
    if(fatal){
        std::exit(EXIT_FAILURE);
    }
}

void error(std::string msg, std::string sol,int line_no, bool fatal){
    errorAttempts = true;
    std::cerr << red << "[ERROR] " << msg << " in block that ends in line "<< line_no << norm << "\n";
    std::cerr << green << "[FIX] " << sol << norm << "\n";
    if(fatal){
        std::exit(EXIT_FAILURE);
    }
}

void warning(std::string msg, int line_no){
    if(verbose){
        std::cerr << magenta << "[WARNING] " << msg << " in block that ends in line "<< line_no << norm << "\n";
    }
}

void warning(std::string msg){
    if(verbose){
        std::cerr << magenta << "[WARNING] " << msg << norm << "\n";
    }
}