#ifndef TREE_HPP
#define TREE_HPP

#include "blocks.hpp"
#include<set>

class Tree{
    private:
        std::set<int64> numbers;
        std::map<std::string, std::shared_ptr<Variable>> variables;
        std::vector<std::shared_ptr<CodeBlock>> roots;
    public:
        Tree(){};
        void setRoots(std::vector<std::shared_ptr<CodeBlock>> newRoots);
        std::map<std::string, std::shared_ptr<Variable>> &getVariables();
        std::set<int64> &getNumbers();
        std::vector<std::shared_ptr<CodeBlock>> &getRoots();
};
#endif