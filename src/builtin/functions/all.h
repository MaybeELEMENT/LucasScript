#ifndef FUNCTION_ALL_H
#define FUNCTION_ALL_H
#include <unordered_map>
#include <variable/variable.h>

namespace Functions {
    Variable log(std::vector<Variable> args, const Token& curToken);
    Variable typeof_func(std::vector<Variable> args, const Token& curToken);
    inline std::unordered_map<std::string, Variable(*)(std::vector<Variable>, const Token&)> functions = {
        {"log", log},
        {"typeof", typeof_func}
    };
};


#endif