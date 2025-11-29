#include "all.h"
#include <parse/parser.h>

Variable getTypeString(Variable& var) {
    switch(var.getType()) {
        case Variable::INTEGER:
            return Variable(std::to_string(var.getValue<long>()));
        case Variable::BOOLEAN:
            return Variable((var.getValue<bool>() ? "true" : "false"));
        case Variable::DECIMAL:
            return Variable(std::to_string(var.getValue<double>()));
        case Variable::STRING:
            return Variable(var.getValue<std::string>());
        case Variable::NULL_TYPE:
            return Variable("null");
        case Variable::REFERENCE:
            return getTypeString(*var.getReferenced());
        default:
            throw ParserException(ParserException::INVALID_VALUE, "cannot convert " + var.getName() + " to string", 0, 0);
    }
}

Variable Functions::string(std::vector<Variable> args, const Token& curToken)
{
    if (args.size() != 1) {
        std::string msg;
        msg = "function requires 1 argument, but ";
        msg += std::to_string(args.size());
        msg += " were provided";
        throw ParserException(ParserException::UNMATCH_ARGUMENT, msg, curToken.getLine(), curToken.getCol());
    }
    return getTypeString(args[0]);
    throw ParserException(ParserException::EXPECTED_EXPRESSION, "argument must be a decimal or string", curToken.getLine(), curToken.getCol());
}