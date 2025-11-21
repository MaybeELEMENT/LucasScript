#include "all.h"
#include <parse/parser.h>

Variable Functions::typeof_func(std::vector<Variable> args, const Token& curToken)
{
    if(args.size() != 1) {
        std::string msg;
        msg = "function requires 1 arguments, but ";
        msg += std::to_string(args.size());
        msg += " was provided";
        throw ParserException(ParserException::UNMATCH_ARGUMENT, msg, curToken.getLine(), curToken.getCol());
    }
    return Variable(args[0].getName());
}