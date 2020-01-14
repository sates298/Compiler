#ifndef OPT_HPP
#define OPT_HPP

#include "transitional_state.hpp"
#include "../assembler/assembler_generator.hpp"
#include <algorithm>

void removeLoads();

void smartGeneratingConstants();
bool sortBySecondElement(const std::tuple<std::string, int64>& a,
                         const std::tuple<std::string, int64>& b);
bool sortBySecondReverseElement(const std::tuple<std::string, int64>& a,
                        const std::tuple<std::string, int64>& b);
void smartGeneratingOneMinusConstant(std::tuple<std::string, int64> num,
                                std::map<int64,std::string> &generated, int64 lastValue);
void smartGeneratingOnePlusConstant(std::tuple<std::string, int64> num,
                                std::map<int64,std::string> &generated, int64 lastValue);
void generatingDifference(int64 num);
#endif