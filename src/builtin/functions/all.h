#ifndef FUNCTION_ALL_H
#define FUNCTION_ALL_H
#include <unordered_map>
#include <variable/variable.h>

namespace Functions {
    Variable print(std::vector<Variable> args, const Token& curToken);
    Variable printl(std::vector<Variable> args, const Token& curToken);
    Variable typeof_func(std::vector<Variable> args, const Token& curToken);
    Variable input(std::vector<Variable> args, const Token& curToken);
    Variable sleep(std::vector<Variable> args, const Token& curToken);
    Variable toInt(std::vector<Variable> args, const Token& curToken);
    Variable toString(std::vector<Variable> args, const Token& curToken);
    Variable sys_run(std::vector<Variable> args, const Token& curToken);
    inline std::unordered_map<std::string, Variable(*)(std::vector<Variable>, const Token&)> functions = {
        {"printl", printl},
        {"print", print},
        {"typeof", typeof_func},
        {"input", input},
        {"sleep", sleep},
        {"toInt", toInt},
        {"sysRun", sys_run}
    };
};


#endif