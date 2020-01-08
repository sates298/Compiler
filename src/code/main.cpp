#include "../headers/program.hpp"

bool optimization = false;

int main(int argc, const char* argv[]){


    run();
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
    for(const auto& c: code){
        log(c->toString());
    }
    return 0;
}