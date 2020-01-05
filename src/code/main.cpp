#include "../headers/main.hpp"

int main(int argc, const char* argv[]){

    yyparse();
    
    
    for(const auto& [k, v]:tree.getVariables()){
        log(v.get()->toString());
    }
    for(const auto& r:tree.getRoots()){
        log(r.get()->toString());
    }
    return 0;
}