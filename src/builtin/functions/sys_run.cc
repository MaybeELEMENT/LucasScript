#include "all.h"
#include <parse/parser.h>
#include <iostream>
#include <unistd.h>

Variable Functions::sys_run(std::vector<Variable> args, const Token& curToken)
{
    if(args.size() != 1) {
        std::string msg;
        msg = "function requires 1 arguments, but ";
        msg += std::to_string(args.size());
        msg += " was provided";
        throw ParserException(ParserException::UNMATCH_ARGUMENT, msg, curToken.getLine(), curToken.getCol());
    }
    if(args[0].getType() != Variable::STRING) {
        throw ParserException(ParserException::EXPECTED_EXPRESSION, "argument must be a string", curToken.getLine(), curToken.getCol());
    }
    return Variable((long)::system(args[0].getValue<std::string>().c_str()));
}