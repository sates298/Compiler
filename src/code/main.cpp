#include "../yacc.hpp"
#include "../headers/transitional/validator.hpp"

int main(int argc, const char* argv[]){

    yyparse();
    
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

    valid();
    return 0;
}