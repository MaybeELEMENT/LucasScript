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
    Variable t_int(std::vector<Variable> args, const Token& curToken);
    Variable string(std::vector<Variable> args, const Token& curToken);
    Variable t_bool(std::vector<Variable> args, const Token& curToken);
    Variable t_decimal(std::vector<Variable> args, const Token& curToken);
    Variable sys_run(std::vector<Variable> args, const Token& curToken);
    inline std::unordered_map<std::string, Variable(*)(std::vector<Variable>, const Token&)> functions = {
        {"printl", printl},
        {"print", print},
        {"typeof", typeof_func},
        {"input", input},
        {"sleep", sleep},
        {"int", t_int},
        {"sysRun", sys_run},
        {"string", string},
        {"bool", t_bool},
        {"decimal", t_decimal},
    };
};


#endif