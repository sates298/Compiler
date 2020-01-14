#include "../../headers/transitional/pseudo_optimization.hpp"

void optimizeCode(){
    //todo 
    //generowanie stałych == dupa zbita
    //optymalizacja modulo/dzielenie
    //w mul zawsze jest wieksza*mniejsza
    //storowac wynik działania zamiast liczenia
    removeLoads();
}

void removeLoads(){
    std::vector<int> idx;
    for(int i=1; i<code.size()-1; i++){
        auto curr = code[i], prev = code[i-1];
        if(curr->getInstr() == LOAD && prev->getInstr() == STORE && curr->getArgument() == prev->getArgument()){
            if(!curr->isJumped()){
                idx.emplace_back(i);
            }
        }
    }
    for(int i=0; i<idx.size(); i++){
        uint64 from = idx[i] - i;
        code.erase(code.begin() + from);
        PseudoAsm::shiftCode(from,-1,code);
    }
}


bool sortBySecondElement(const std::tuple<std::string, int64>& a,
                        const std::tuple<std::string, int64>& b){
    return  (std::get<1>(a) < std::get<1>(b));
}

bool sortBySecondReverseElement(const std::tuple<std::string, int64>& a,
                        const std::tuple<std::string, int64>& b){
    return  (std::get<1>(a) > std::get<1>(b));
}

void generatingDifference(int64 num){
    int64 tmp = abs(num);
    if(abs(num) < 23){
        while(tmp){
            if(num < 0){
                _PUSH_INS(DEC, 0)
            }else{
                _PUSH_INS(INC, 0)
            }
            tmp--;
        }
    }else{
        std::vector<char> gen;
        while(tmp){
            if(tmp&1){
                gen.emplace_back('i');
            }
            tmp >>=1;
            if(tmp){
                gen.emplace_back('s');
            }
        }
        for(int i = gen.size()-1; i>=0; i--){
            if(gen[i] == 's'){
                _PUSH_PSEUDO_INS(SHIFT, "1");
            }else{
                if(num < 0){
                    _PUSH_INS(DEC, 0)
                }else{
                    _PUSH_INS(INC, 0)
                }
            }
        }
    }
}

void smartGeneratingOneMinusConstant(std::tuple<std::string, int64> num,
                                std::map<int64, std::string> &generated, int64 lastValue){
    int64 curr = std::get<1>(num);
    std::string currReg = std::get<0>(num);
    int64 tmp = curr;
    bool finished = false;
    int64 diff = tmp - lastValue;
    int64 div = lastValue != 0 ? tmp/lastValue : 0;
    bool saved = true;
    while(!finished){
        if(abs(diff) < 10){
            generatingDifference(diff);
            finished = true;
        }else{
            auto it = generated.find(diff);
            auto it1 = generated.find((-1)*(diff));
            if(it != generated.end()){
                _PUSH_PSEUDO_INS(ADD, generated[diff]);
                finished = true;
            }else if(it1 != generated.end()){
                _PUSH_PSEUDO_INS(SUB, generated[(-1)*(diff)]);
                finished = true;
            }else if(div > 3){
                saved = false;
                _PUSH_PSEUDO_INS(SHIFT, "1")
                _PUSH_PSEUDO_INS(SHIFT, "1")
                lastValue *= 4;
                diff = tmp - lastValue;
                div = tmp/lastValue;
            }else{
                if(diff != 0){
                    if(!saved){
                        _PUSH_PSEUDO_INS(STORE, "TMP1exp")
                    }
                    _PUSH_INS(SUB, 0);
                    generatingDifference(diff);
                    if(saved){
                        _PUSH_PSEUDO_INS(ADD, generated[lastValue]);
                    }else{
                        _PUSH_PSEUDO_INS(ADD, "TMP1exp")
                    }
                    
                }
                finished = true;
            }
        }
    }
    generated[curr] = currReg;
    _PUSH_PSEUDO_INS(STORE, currReg)
}

void smartGeneratingOnePlusConstant(std::tuple<std::string, int64> num,
                                std::map<int64, std::string> &generated, int64 lastValue){
    int64 curr = std::get<1>(num);
    std::string currReg = std::get<0>(num);
    int64 tmp = curr;
    bool finished = false;
    int64 diff = tmp - lastValue;
    int64 div = tmp/lastValue;
    bool saved = true;
    while(!finished){
        if(diff < 10){
            generatingDifference(diff);
            finished = true;
        }else{
            auto it = generated.find(diff);
            if(it != generated.end()){
                _PUSH_PSEUDO_INS(ADD, generated[diff]);
                finished = true;
            }else if(div > 3){
                saved = false;
                _PUSH_PSEUDO_INS(SHIFT, "1")
                _PUSH_PSEUDO_INS(SHIFT, "1")
                lastValue *= 4;
                diff = tmp - lastValue;
                div = tmp/lastValue;
            }else{
                if(diff > 0){
                    if(!saved){
                        _PUSH_PSEUDO_INS(STORE, "TMP1exp")
                    }
                    _PUSH_INS(SUB, 0);
                    generatingDifference(diff);
                    if(saved){
                        _PUSH_PSEUDO_INS(ADD, generated[lastValue]);
                    }else{
                        _PUSH_PSEUDO_INS(ADD, "TMP1exp")
                    }
                    finished = true;
                }else{
                    finished = true;
                }
            }
        }
    }

    generated[curr] = currReg;
    _PUSH_PSEUDO_INS(STORE, currReg)
}

void smartGeneratingConstants(){
    std::vector<std::tuple<std::string, int64>> constsMinus;
    std::vector<std::tuple<std::string, int64>> constsPlus;
    for(auto &[k,r]:registers){
        if(r->isNumber &&( r->name != "1" && r->name != "-1")){
            std::string reg = r->name;
            int64 curr;
            bool sign = false;
            if(r->isOffset){
                curr = r->offsetVal;
            }else{
                curr = std::stoll(r->name);
            }
            if(curr < 0){
                constsMinus.emplace_back(std::make_tuple(reg, curr));
            }else{
                constsPlus.emplace_back(std::make_tuple(reg, curr));
            }
            
        }
    }
    std::sort(constsPlus.begin(), constsPlus.end(), sortBySecondElement);
    std::sort(constsMinus.begin(), constsMinus.end(), sortBySecondReverseElement);
    std::map<int64,std::string> generated;
    int64 last = 1;
    generated[1] = "1";
    generated[-1] = "-1";

    _PUSH_INS(DEC, 0)
    _PUSH_PSEUDO_INS(STORE, "-1");
    _PUSH_INS(INC, 0)
    _PUSH_INS(INC, 0)
    _PUSH_PSEUDO_INS(STORE, "1");
    for(auto& tup: constsPlus){
        smartGeneratingOneMinusConstant(tup, generated, last);
        last = std::get<1>(tup);
    }
    _PUSH_INS(SUB, 0)
    last = 0;
    for(auto& tup:constsMinus){
        smartGeneratingOneMinusConstant(tup, generated, last);
        last = std::get<1>(tup);
    }
}

