#include "parser.h"
extern Variable parseMultiplicative(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseAdditive(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched)
{
    Variable left = parseMultiplicative(tokens, i, env, isLValueContext, mode, chainMatched);

    while (i < tokens.size())
    {
        const Token& token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::OPERATOR ||
            (token.getValue() != "+" && token.getValue() != "-"))
            break;

        i++;
        Variable right = parseMultiplicative(tokens, i, env, isLValueContext, mode, chainMatched);

        if (token.getValue() == "+")
        {
            left = left.add(right, token);
        }
        else
        {
            left = left.subtract(right, token);
        }
    }

    return left;
}