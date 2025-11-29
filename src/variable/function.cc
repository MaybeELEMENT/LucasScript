#include "function.h"

bool Function::isDefined() {
    return defined;
}
std::vector<FunctionArgument> Function::getParams() {
    return params;
}
std::vector<Token> Function::getBody() {
    return body;
}
std::string FunctionArgument::getName() {
    return this->name;
}
bool FunctionArgument::getIsRef() {
    return this->isRef;
}