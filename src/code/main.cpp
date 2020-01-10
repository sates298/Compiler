#include "../headers/program.hpp"
#include <unistd.h>

bool optimization = false;
bool debug = false;

int main(int argc, const char* argv[]){

    std::string curr, in = "", out = "";
    for(int i=1; i<argc; i++){
        curr = std::string(argv[i]);
        if(curr.find(".imp") != std::string::npos){
            if(access(curr.c_str(), R_OK) != -1){
                freopen(curr.c_str(),"r",stdin);
                in = curr;
            }else{
                error("Can't open input file", true);
            }
        }else if(curr.find(".mr") != std::string::npos){
            if(access(curr.c_str(), W_OK) != -1){
                freopen(curr.c_str(),"w",stdout);
                out = curr;
            }else{
                error("Can't open input file", true);
            }
        }else if(curr.find("-") == 0){
            if(curr.find("o") != std::string::npos){
                optimization = true;
            }
            if(curr.find("d") != std::string::npos){
                debug = true;
            }
        }else{
            error("Unrecognized command line option " + curr, true);
        }
    }
    if(in == ""){
        error("No input file",true);
    }
    if(out == ""){
        error("No output file", true);
    }

    run();
    fclose(stdin);
    if(!errorAttempts){
        for(auto& i: finalCode){
            std::cout << i->toString() << "\n";
        }
    }
    fclose(stdout);

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
    for(const auto& c: code){
        log(c->toString());
    }
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