#include "parser.h"
#include <unordered_map>
extern Variable parseExpression(const std::vector<Token> &tokens, std::shared_ptr<Environment> env, Mode mode, bool &chainMatched);

/**
 * parsingLevel:
 * 0 - global
 * 1 - function
 * 2 - if/elif/else
 * -1 - reuse parentEnv as local (for loops)
 */
Variable parse(const std::vector<Token>& tokens, int8_t parsingLevel = 1, std::shared_ptr<Environment> parentEnv = nullptr, const std::unordered_map<std::string, Variable>& extras = {})
{
    std::shared_ptr<Environment> localEnv;

    if (parsingLevel > 0)
    {
        localEnv = std::make_shared<Environment>();
        localEnv->parent = parentEnv;

        for (const auto &[key, val] : extras) {
            localEnv->vars[key] = val; 
        }
    }
    else if (parsingLevel == -1) {
        localEnv = parentEnv;
        for (const auto &[key, val] : extras) {
            localEnv->vars[key] = val; 
        }
    }
    else
        localEnv = parentEnv;

    if (tokens.empty())
        return Variable(); // return Null

    std::vector<Token> expression;
    Mode mode = Mode::NONE;
    Mode lastMode = Mode::NONE;
    bool chainMatched = false;

    for (const auto& token : tokens)
    {
        bool shouldExecute = false;

        switch (token.getType())
        {
        case Token::END_OF_LINE:
        case Token::OPERATOR:
            if ((token.getType() == Token::OPERATOR && token.getValue() == ";") || token.getType() == Token::END_OF_LINE)
            {
                shouldExecute = true;
                break; 
            }
        default:
            if (expression.empty() && token.getType() == Token::KEYWORD)
            {
                if (token.getValue() == "ref")
                    mode = Mode::REFERENCE;
                else if (token.getValue() == "return")
                    mode = Mode::RETURN;
                else if (token.getValue() == "if") {
                    mode = Mode::IF;
                    chainMatched = false;
                    lastMode = mode;
                }
                else if (token.getValue() == "elif") {
                    mode = Mode::ELIF;
                    if(lastMode != Mode::IF && lastMode != Mode::ELIF)
                        throw ParserException(ParserException::UNEXPECTED_SYNTAX, "'elif' without preceding if statement", token.getLine(), token.getCol());
                    lastMode = mode; 
                }
                else if (token.getValue() == "else") {
                    mode = Mode::ELSE;
                    if(lastMode != Mode::IF && lastMode != Mode::ELIF)
                        throw ParserException(ParserException::UNEXPECTED_SYNTAX, "'else' without preceding if or elif statement", token.getLine(), token.getCol());
                    lastMode = mode;
                }
                else if (token.getValue() == "while") {
                    mode = Mode::WHILE;
                    chainMatched = false;
                    lastMode = Mode::NONE;
                }
                else if (token.getValue() == "break") {
                    mode = Mode::BREAK;
                }
                break;
            }
            expression.push_back(token);
            
            if (token.getType() == Token::BRACKETS && token.getValue() == "{") {
                shouldExecute = true;
            }
            break;
        }
        
        if (shouldExecute)
        {
             if (!expression.empty() || mode == Mode::RETURN || mode == Mode::BREAK) {
                if(mode == RETURN)
                {
                    Variable res = parseExpression(expression, localEnv, mode, chainMatched);
                    if (parsingLevel > 1) {
                        throw ReturnException(res);
                    }
                    return res;
                }
                // Handle BREAK command
                if(mode == BREAK)
                {
                    throw BreakException(token.getLine(), token.getCol());
                }

                try {
                    parseExpression(expression, localEnv, mode, chainMatched);
                } catch (ReturnException& e) {
                    if (parsingLevel > 1) throw e;
                    return e.value;
                } catch (BreakException& e) {
                    // If we are at function level (1) or global level (0) and catch a break, it's invalid.
                    // Note: parsingLevel -1 (loop) or 2 (if/else) should propagate the break.
                    if (parsingLevel != -1 && parsingLevel <= 1) {
                        throw ParserException(ParserException::UNEXPECTED_SYNTAX, "'break' outside of loop", e.line, e.col);
                    }
                    throw e; // Propagate to parent loop
                }
                
                if (mode != IF && mode != ELIF && mode != ELSE) {
                    chainMatched = false;
                    lastMode = Mode::NONE;
                }
             }
             expression.clear();
             mode = Mode::NONE;
        }
    }
    try {
        if(mode == RETURN)
        {
            Variable res = parseExpression(expression, localEnv, mode, chainMatched);
            if (parsingLevel > 1) {
                throw ReturnException(res);
            }
            return res;
        }
        // Handle trailing break at end of block
        if(mode == BREAK)
        {
            // We don't have the exact token here easily, passing 0,0 or need to track last token
            // However, the previous loop logic usually handles execution. This is for dangling expression.
            throw BreakException(0, 0);
        }
        return parseExpression(expression, localEnv, mode, chainMatched);
    } catch (ReturnException& e) {
        if (parsingLevel > 1) throw e;
        return e.value;
    } catch (BreakException& e) {
        if (parsingLevel != -1 && parsingLevel <= 1) {
            throw ParserException(ParserException::UNEXPECTED_SYNTAX, "'break' may only be used within a loop or switch", e.line, e.col);
        }
        throw e;
    }
}
Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
}

void Parser::start()
{
    std::shared_ptr<Environment> globalEnv = std::make_shared<Environment>();
    Variable var = parse(this->tokens, true, globalEnv);
}