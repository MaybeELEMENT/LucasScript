#include "parser.h"
extern Variable parseUnary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseMultiplicative(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched)
{
    Variable left = parseUnary(tokens, i, env, isLValueContext, mode, chainMatched);

    while (i < tokens.size())
    {
        const Token& token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::OPERATOR ||
            (token.getValue() != "*" && token.getValue() != "/" && token.getValue() != "%"))
            break;

        i++;
        Variable right = parseUnary(tokens, i, env, isLValueContext, mode, chainMatched);

        if (token.getValue() == "*")
            left = left.mutliply(right, token);
        else if (token.getValue() == "%")
            left = left.modulus(right, token);
        else
            left = left.divide(right, token);
    }

    return left;
}