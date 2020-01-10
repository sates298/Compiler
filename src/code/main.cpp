#include "../headers/program.hpp"

bool optimization = false;

int main(int argc, const char* argv[]){

    //todo otwieranie i czytanie z pliku
    run();
    if(!errorAttempts){
        for(auto& i: finalCode){
            //todo pisanie do pliku
        }
    }

    // for(const auto& c:tree.getNumbers()){
    //     log(std::to_string(c));
    // }
    // for(const auto& [k, v]:tree.getVariables()){
    //     log(v->toString());
    // }
    // for(const auto& r:tree.getRoots()){
    //     log(r->toString());
    //     std::clog << "root " << r << r->toString() << "\n";
    // }
    log("pseudo assembler");
    // for(const auto& c: code){
    //     log(c->toString());
    // }
    log("registers");
    for(const auto& [k, r]: registers){
        log(r->toString()); //todo jakis seg fault
    }
    log("real assembler")
    // for(const auto& a: finalCode){
    //     log(a->toString());
    // }
    return 0;
}