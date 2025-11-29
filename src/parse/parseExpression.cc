#include "parser.h"


extern Variable parseLogical(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
extern Variable parseAssignment(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, Mode mode, bool &chainMatched);
Variable parseExpression(const std::vector<Token> &tokens, std::shared_ptr<Environment> env, Mode mode, bool &chainMatched)
{
    if (tokens.empty()) {
        return Variable();
    }

    if (mode == IF || mode == ELIF || mode == ELSE || mode == WHILE) {
        size_t i = 0;
        return parseLogical(tokens, i, env, false, mode, chainMatched);
    }

    size_t i = 0;
    Variable res = parseAssignment(tokens, i, env, mode, chainMatched);

    if (i < tokens.size())
    {
        std::string msg;
        msg = "unexpected syntax '";
        msg += tokens[i].getValue();
        msg += "'";
        throw ParserException(ParserException::UNEXPECTED_SYNTAX, msg, tokens[i].getLine(), tokens[i].getCol());
    }
    return res;
}