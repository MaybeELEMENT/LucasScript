#include "all.h"
#include <parse/parser.h>

Variable Functions::t_bool(std::vector<Variable> args, const Token& curToken)
{
    if (args.size() != 1) {
        std::string msg;
        msg = "function requires 1 argument, but ";
        msg += std::to_string(args.size());
        msg += " were provided";
        throw ParserException(ParserException::UNMATCH_ARGUMENT, msg, curToken.getLine(), curToken.getCol());
    }
    if (args[0].getType() == Variable::INTEGER) {
        return Variable(static_cast<bool>(args[0].getValue<long>()));
    }
    if (args[0].getType() == Variable::DECIMAL) {
        return Variable(static_cast<bool>(args[0].getValue<double>()));
    }

    if(args[0].getType() == Variable::STRING) {
        try {
            return Variable(args[0].getValue<std::string>() == "true");
        }
        catch(std::invalid_argument e) {
            std::string msg;
            msg = "cannot convert string '";
            msg += args[0].getValue<std::string>();;
            msg += "' to integer";
            throw ParserException(ParserException::INVALID_VALUE, msg, curToken.getLine(), curToken.getCol());
        }
    }
    throw ParserException(ParserException::EXPECTED_EXPRESSION, "argument must be an int, decimal or string", curToken.getLine(), curToken.getCol());
}