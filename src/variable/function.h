#ifndef FUNCTION_H
#define FUNCTION_H
#include <parse/lexer.h>
#include <vector>
#include <string>
class FunctionArgument {
    std::string name;
    bool isRef;
public:
    FunctionArgument(std::string name, bool isRef) : name(name), isRef(isRef) {}
    FunctionArgument() {}
    std::string getName();
    bool getIsRef();
};
class Function {
    bool defined = false;
    std::vector<FunctionArgument> params;
    std::vector<Token> body;
public:
    Function() {}
    Function(std::vector<Token> fbody, std::vector<FunctionArgument> fparams) : defined(true), params(fparams), body(fbody) {}

    bool isDefined();
    std::vector<FunctionArgument> getParams();
    std::vector<Token> getBody();
};
#endif