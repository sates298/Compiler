#ifndef VAR_HPP
#define VAR_HPP
#include<string>
#include<memory>
#include<map>
#include "typedefs.hpp"

enum VariableType{
    VAL, ARR
};

class Variable{
    protected:
        std::string name;
        long long value;
        bool constant = false;
        bool declared = false;
        VariableType type;
    public:
        Variable(std::string name);
        ~Variable(){}

        void setValue(long long value);
        void setConstant(bool constant);
        void setDeclared(bool declared);

        std::string getName();
        long long getValue();
        bool isConstant();
        bool isDeclared();
        bool isArray();

        std::string toString();

};

class ArrayVariable : public Variable{
    private:
        std::map<long long, std::shared_ptr<Variable>> elements;
        long long firstIdx;
        long long lastIdx;
    public:
        ArrayVariable(std::string name ,long long a, long long b);
        void setElement(long long idx, std::shared_ptr<Variable> element);
        std::shared_ptr<Variable> getElement(long long idx);
        std::string toString();
};

struct Call{
    std::string name = "";
    bool isFirstIndex;

    long long firstIdx;
    std::string secondIdx = "";

    std::string toString();
};

struct Value{
    std::shared_ptr<Call> cal = nullptr;
    long long val;

    std::string toString();
};

#endif