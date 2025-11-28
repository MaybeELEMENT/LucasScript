// By Lucas
#ifndef VARIABLE_H
#define VARIABLE_H
#include <variant>
#include <string>
#include <memory>
#include "class.h"
#include <parse/lexer.h>
#include "function.h"


struct Null {};

class Reference;
class Variable;
class Class;  // forward declare

class Reference {
    Variable* ref;  // pointer to another Variable
public:
    Variable* get();
    Reference(Variable* v) : ref(v) {}
};

class Variable {
public:
    enum Type {
        INTEGER,
        DECIMAL,
        STRING,
        BOOLEAN,
        CLASS,
        NULL_TYPE,
        REFERENCE,
        FUNCTION
    };
private:
    std::variant<long, double, std::string, bool, Null, Reference, std::shared_ptr<Class>, Function> value;
    Type type;
public:
    Type getType() const;
    Type getRefType();
    template<typename T>
    T getValue() {
        if (type == REFERENCE) {
            return std::get<Reference>(value).get()->getValue<T>();
        }
        return std::get<T>(value);
    }
    Variable(long v) : value(v), type(INTEGER) {}
    Variable(int v) : value((long)v), type(INTEGER) {}
    Variable(double v) : value(v), type(DECIMAL) {}
    Variable(const std::string& v) : value(v), type(STRING) {}
    Variable(const char* v) : value(std::string(v)), type(STRING) {}
    Variable(bool v) : value(v), type(BOOLEAN) {}
    Variable(std::shared_ptr<Class> v) : value(v), type(CLASS) {}
    Variable(Function v) : value(v), type(FUNCTION) {}
    Variable()  : value(Null{}), type(NULL_TYPE) {}
    Variable(const Variable& other) : value(other.value), type(other.type) {}
    std::string getName();
    static std::string getName(const Variable& variable);
    static std::string getName(const Type& type);

    Variable compareEqual(Variable& b, const Token& curToken);
    
    Variable compareInequal(Variable& b, const Token& curToken);
    Variable compareAnd(Variable& b, const Token& curToken);
    Variable compareOr(Variable& b, const Token& curToken);
    Variable compareGreater(Variable& b, const Token& curToken);
    Variable compareGreaterEqual(Variable& b, const Token& curToken);
    Variable compareSmaller(Variable& b, const Token& curToken);
    Variable compareSmallerEqual(Variable& b, const Token& curToken);
    Variable add(Variable& b, const Token& curToken);
    Variable subtract(Variable& b, const Token& curToken);
    Variable mutliply(Variable& b, const Token& curToken);
    Variable divide(Variable& b, const Token& curToken);
    Variable modulus(Variable& b, const Token& curToken);
    Variable reverse(const Token& curToken);
    Variable* getReferenced() {
        if (type == REFERENCE) {
            Variable* var = std::get<Reference>(value).get();
            if(var->type == REFERENCE) {
                return var->getReferenced();
            }
            return var;
        }
        return this;
    }

    Variable& operator=(const Variable& rhs) {
        Variable* target = getReferenced();
        target->value = rhs.value;
        target->type = rhs.type;
        return *target;
    }

    static Variable makeRef(Variable& v);
    bool isNull();

    void setValue(long v);
    void setValue(double v);
    void setValue(std::string v);
    void setValue(std::shared_ptr<Class> v);
    void setValue();
    void setValue(bool v);
};

struct Environment {
    std::unordered_map<std::string, Variable> vars;
    std::shared_ptr<Environment> parent;

    Variable* find(const std::string& name) {
        if (auto it = vars.find(name); it != vars.end())
            return &it->second;
        if (parent)
            return parent->find(name);
        return nullptr;
    }
};


#endif