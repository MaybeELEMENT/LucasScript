#include "parser.h"

extern Variable parseRelational(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseEquality(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched)
{
    Variable left = parseRelational(tokens, i, env, isLValueContext, mode, chainMatched);

    while (i < tokens.size())
    {
        const Token& token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::COMPARISON ||
            (token.getValue() != "==" && token.getValue() != "!="))
            break;
        i++;
        Variable right = parseRelational(tokens, i, env, isLValueContext, mode, chainMatched);

        if (token.getValue() == "==")
            left = (left.compareEqual(right, token));
        else
            left = (left.compareInequal(right, token));
    }

    return left;
}