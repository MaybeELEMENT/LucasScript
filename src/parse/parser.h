#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "lexer.h"
#include <variable/variable.h>

namespace Parser_Def {
    inline const std::vector<std::string> keywords = {
        "import",
        "return",
        "break",
        "while",
        "loop",
        "until",
        "del",
        "ref",
        "if",
        "else",
        "elif",
        "match",
        "case",
        "execute",
        "class",
        "this",
        "and",
        "or"
    };
};

class ParserException {
public:
    enum ParserErrorType {
        UNEXPECTED_SYNTAX,
        UNKNOWN_ERROR,
        DIVIDE_BY_ZERO,
        INVALID_LVALUE,
        UNDEFINED_VARIABLE,
        EXPECTED_EXPRESSION,
        EXPECTED_SYNTAX,
        NOT_CALLABLE,
        UNMATCH_ARGUMENT,
        UNMATCH_OPERAND,
        UNSUPPORTED_OPERATOR
    };
    ParserErrorType errorType;
    std::string msg;
    unsigned int line = 0;
    unsigned int column = 0;
    std::string errorCode() {
        switch(errorType) {
            case UNEXPECTED_SYNTAX:
                return "UNEXPECTED_SYNTAX";
            case DIVIDE_BY_ZERO:
                return "DIVIDE_BY_ZERO";
            case INVALID_LVALUE:
                return "INVALID_LVALUE";
            case UNDEFINED_VARIABLE:
                return "UNDEFINED_VARIABLE";
            case EXPECTED_EXPRESSION:
                return "EXPECTED_EXPRESSION";
            case EXPECTED_SYNTAX:
                return "EXPECTED_SYNTAX";
            case NOT_CALLABLE:
                return "NOT_CALLABLE";
            case UNMATCH_ARGUMENT:
                return "UNMATCH_ARGUMENT";
            case UNMATCH_OPERAND:
                return "UNMATCH_OPERAND";
            case UNSUPPORTED_OPERATOR:
                return "UNSUPPORTED_OPERATOR";
            default:
                return "UNKNOWN_ERROR";
        }
    }
    ParserException(ParserErrorType error, std::string message, unsigned int eline, unsigned int ecolumn)
        : errorType(error), msg(message), line(eline), column(ecolumn) {}
};

class Parser {
    std::vector<Token> tokens;
public:
    Parser(std::vector<Token> tokens);

    void start();
};

#endif