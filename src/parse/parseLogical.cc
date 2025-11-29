#include "parser.h"

extern Variable parseEquality(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseLogical(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched)
{
    Variable left = parseEquality(tokens, i, env, isLValueContext, mode, chainMatched);

    while (i < tokens.size())
    {
        const Token& token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::KEYWORD || (token.getValue() != "and" && token.getValue() != "or"))
            break;

        i++;

        Variable right = parseEquality(tokens, i, env, isLValueContext, mode, chainMatched);

        if (token.getValue() == "and")
            left = (left.compareAnd(right, token));
        else
            left = (left.compareOr(right, token));
    }
    
    return left;
}