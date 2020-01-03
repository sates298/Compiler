#ifndef VAR_HPP
#define VAR_HPP
#include<string>
#include<memory>
#include<map>

class Variable{
    protected:
        std::string name;
        long long value;
        bool constant = false;
        bool declared = false;
        bool array = false;
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

};

struct value{
    std::shared_ptr<Variable> var = nullptr;
    long long val;
};

#endif