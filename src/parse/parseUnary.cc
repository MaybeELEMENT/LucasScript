#include "parser.h"

extern Variable parsePrimary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);

Variable parseUnary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched)
{
    if (i < tokens.size())
    {
        const Token& token = tokens[i];
        if (token.getType() == Token::OPERATOR && token.getValue() == "!")
        {
            i++;
            Variable operand = parseUnary(tokens, i, env, false, mode, chainMatched);
            return operand.reverse(token);
        }
    }
    return parsePrimary(tokens, i, env, isLValueContext, mode, chainMatched);
}