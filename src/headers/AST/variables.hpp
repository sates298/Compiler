#ifndef VAR_HPP
#define VAR_HPP
#include<string>
#include<memory>
#include<map>
#include "../typedefs.hpp"

enum VariableType{
    VAL, ARR
};

class Variable{
    protected:
        std::string name;
        int64 value;
        bool constant = false;
        bool declared = false;
        VariableType type;
    public:
        Variable(std::string name);
        ~Variable(){}

        void setValue(int64 value);
        void setConstant(bool constant);
        void setDeclared(bool declared);

        std::string getName();
        int64 getValue();
        bool isConstant();
        bool isDeclared();
        bool isArray();

        std::string toString();

};

class ArrayVariable : public Variable{
    private:
        std::map<int64, std::shared_ptr<Variable>> elements;
        int64 firstIdx;
        int64 lastIdx;
    public:
        ArrayVariable(std::string name ,int64 a, int64 b);
        void setElement(int64 idx, std::shared_ptr<Variable> element);
        std::shared_ptr<Variable> getElement(int64 idx);
        std::string toString();
};

struct Call{
    int64 line;
    std::string name = "";
    bool isFirstIndex;

    int64 firstIdx;
    std::string secondIdx = "";

    std::string toString();
};

struct Value{
    std::shared_ptr<Call> cal = nullptr;
    int64 val;

    std::string toString();
};

#endif