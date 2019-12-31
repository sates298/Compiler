#ifndef VAR_HPP
#define VAR_HPP
#include<string>
#include<memory>
#include<map>

class Variable{
    protected:
        std::string name;
        long long value;
        bool clean = true;
        bool declared = false;
        bool array = false;
    public:
        Variable(std::string name);
        ~Variable(){}

        void setValue(long long value);
        void setClean(bool clean);
        void setDeclared(bool declared);

        std::string getName();
        long long getValue();
        bool isClean();
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

#endif