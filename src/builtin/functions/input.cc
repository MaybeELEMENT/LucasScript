#include "all.h"
#include <parse/parser.h>
#include <iostream>

Variable Functions::input(std::vector<Variable> args, const Token& curToken)
{
    Functions::print(args, curToken);
    std::string in;
    std::getline(std::cin, in);
    return Variable(in);
}