// By Lucas
#include "class.h"
void Class::setMember(const std::string& name, Variable& var) {
    if(members.find(name) == members.end()) {
        members.emplace(name, var);
        return;
    }
    members[name] = var;
}
Variable Class::getMember(const std::string& name) {
    if(members.find(name) != members.end()) {
        return members[name];
    }
    return Variable(); // return null variable if not found
}
std::string Class::getName() {
    return name;
}