#include "function.h"

bool Function::isDefined() {
    return defined;
}
std::vector<std::string> Function::getParams() {
    return params;
}
std::vector<Token> Function::getBody() {
    return body;
}