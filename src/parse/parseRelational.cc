#include "parser.h"
extern Variable parseAdditive(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);

Variable parseRelational(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched)
{
    Variable left = parseAdditive(tokens, i, env, isLValueContext, mode, chainMatched);

    while (i < tokens.size())
    {
        const Token& token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::COMPARISON ||
            (token.getValue() != "<" && token.getValue() != ">" &&
             token.getValue() != "<=" && token.getValue() != ">="))
            break;

        i++;
        Variable right = parseAdditive(tokens, i, env, isLValueContext, mode, chainMatched);

        if (token.getValue() == "<")
            left = (left.compareSmaller(right, token));
        else if (token.getValue() == ">")
            left = (left.compareGreater(right, token));
        else if (token.getValue() == "<=")
            left = (left.compareSmallerEqual(right, token));
        else
            left = (left.compareGreaterEqual(right, token));
    }
    return left;
}