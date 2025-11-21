#ifndef FUNCTION_H
#define FUNCTION_H
#include <parse/lexer.h>
#include <vector>
#include <string>
class Function {
    bool defined = false;
    std::vector<std::string> params;
    std::vector<Token> body;
public:
    Function() {}
    Function(std::vector<Token> fbody, std::vector<std::string> fparams) : defined(true), params(fparams), body(fbody) {}

    bool isDefined();
    std::vector<std::string> getParams();
    std::vector<Token> getBody();
};
#endif