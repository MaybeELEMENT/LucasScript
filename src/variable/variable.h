// By Lucas
#ifndef VARIABLE_H
#define VARIABLE_H
#include <variant>
#include <string>
#include <memory>
#include "class.h"
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
        REFERENCE
    };
private:
    std::variant<long, double, std::string, bool, Null, Reference, std::shared_ptr<Class>> value;
    Type type;
public:
    Type getType() const;
    template<typename T>
    T getValue();
    Variable(long v) : value(v), type(INTEGER) {}
    Variable(double v) : value(v), type(DECIMAL) {}
    Variable(const std::string& v) : value(v), type(STRING) {}
    Variable(bool v) : value(v), type(BOOLEAN) {}
    Variable(std::shared_ptr<Class> v) : value(v), type(CLASS) {}
    Variable()  : value(Null{}), type(NULL_TYPE) {}
    static Variable makeRef(Variable& v);
    bool isNull();

    void setValue(long v);
    void setValue(double v);
    void setValue(std::string v);
    void setValue(std::shared_ptr<Class> v);
    void setValue();
    void setValue(bool v);
};


#endif