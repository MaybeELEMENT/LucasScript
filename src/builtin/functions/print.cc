#include "all.h"

void printFunc(Variable& arg, Variable::Type type)
{
    // printf("%s\n", Variable::getName(arg.getRefType()).c_str());
    switch (type)
    {
        case Variable::INTEGER:
            printf("%ld", arg.getValue<long>());
            break;
        case Variable::DECIMAL:
            printf("%f", arg.getValue<double>());
            break;
        case Variable::STRING:
            printf("%s", arg.getValue<std::string>().c_str());
            break;
        case Variable::BOOLEAN:
            printf("%s", arg.getValue<bool>() ? "true" : "false");
            break;
        case Variable::CLASS:
            printf("<class>");
            break;
        case Variable::NULL_TYPE:
            printf("null");
            break;
        case Variable::REFERENCE:
            printFunc(arg, arg.getRefType());
            break;
        case Variable::FUNCTION:
            printf("function");
            break;
    }
}

Variable Functions::printl(std::vector<Variable> args, const Token& curToken)
{
    for (auto arg : args)
    {
        printFunc(arg, arg.getType());
    }
    printf("\n");
    return Variable();
}
Variable Functions::print(std::vector<Variable> args, const Token& curToken)
{
    for (auto arg : args)
    {
        printFunc(arg, arg.getType());
    }
    return Variable();
}