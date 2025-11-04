// By Lucas
#ifndef CLASS_H
#define CLASS_H
#include <unordered_map>
#include <string>
#include "variable.h"
class Variable;
class Class {
    std::string name;
public:
    std::unordered_map<std::string, Variable> members;
    void setMember(const std::string& name, Variable& var);
    Variable getMember(const std::string& name);
    Class(std::string cname) : name(cname) {}
    std::string getName();
};
#endif