#include "parser.h"
#include <unordered_map>
#include <variable/function.h>
#include <builtin/functions/all.h>

struct ReturnException {
    Variable value;
    ReturnException(Variable v) : value(v) {}
};

// Added BreakException to signal loop exit
struct BreakException {
    unsigned int line;
    unsigned int col;
    BreakException(unsigned int l, unsigned int c) : line(l), col(c) {}
};

std::shared_ptr<Environment> globalEnv = std::make_shared<Environment>();

enum Mode
{
    NONE,
    REFERENCE,
    RETURN,
    IF,
    ELIF,
    ELSE,
    WHILE, 
    BREAK // Added BREAK mode
};

Variable parse(const std::vector<Token>& tokens, int8_t parsingLevel = 1, std::shared_ptr<Environment> parentEnv = nullptr, const std::unordered_map<std::string, Variable>& extras = {});

Variable parseUnary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseMultiplicative(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseAdditive(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseRelational(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseEquality(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseLogical(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseAssignment(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, Mode mode, bool &chainMatched);
Variable parsePrimary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);
Variable parseExpression(const std::vector<Token> &tokens, std::shared_ptr<Environment> env, Mode mode, bool &chainMatched);

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
}

void Parser::start()
{
    Variable var = parse(this->tokens, true);
}
struct Term
{
    std::vector<Token> operators;
    std::vector<Token> operands;
};

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

Variable parseAssignment(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, Mode mode, bool &chainMatched)
{
    size_t assignIndex = SIZE_MAX;
    Token assignmentToken;

    for (size_t j = i; j < tokens.size(); ++j)
    {
        if (tokens[j].getType() == Token::ASSIGNMENT)
        {
            assignmentToken = tokens[j];
            assignIndex = j;
            break;
        }
    }

    if (assignIndex != SIZE_MAX)
    {
        if (tokens[assignIndex + 1].getType() == Token::BRACKETS &&
            tokens[assignIndex + 1].getValue() == "{")
        {
            if (i + 1 < assignIndex && tokens[i].getType() == Token::NAME)
            {
                std::string funcName = tokens[i].getValue();
                std::vector<std::string> params;

                if (tokens[i + 1].getType() == Token::BRACKETS &&
                    tokens[i + 1].getValue() == "(")
                {
                    bool comma = false;
                    for (const auto &paramToken : tokens[i + 1].getChildren())
                    {
                        if (comma)
                        {
                            if (paramToken.getType() == Token::OPERATOR && paramToken.getValue() == ",")
                            {
                                comma = false;
                                continue;
                            }
                            else
                            {
                                throw ParserException(ParserException::EXPECTED_SYNTAX, "',' was expected before next parameter",
                                                      paramToken.getLine(), paramToken.getCol());
                            }
                        }
                        else
                        {
                            if (paramToken.getType() == Token::OPERATOR && paramToken.getValue() == ",")
                            {
                                throw ParserException(ParserException::EXPECTED_SYNTAX, "a name was expected before next parameter",
                                                      paramToken.getLine(), paramToken.getCol());
                            }
                        }
                        if (paramToken.getType() == Token::NAME)
                            params.push_back(paramToken.getValue());
                        comma = true;
                    }
                }
                std::vector<Token> body = tokens[assignIndex + 1].getChildren();

                auto func = Function(body, params);
                if (assignmentToken.getValue() != "=")
                    throw ParserException(ParserException::UNEXPECTED_SYNTAX, "function can only be defined with '=' assignment", assignmentToken.getLine(), assignmentToken.getCol());

                env->vars[funcName] = Variable(func);
                i = assignIndex + 2; 
                
                return Variable();
            }
        }

        size_t rightStart = assignIndex + 1;
        size_t rightIndex = rightStart;
        
        Variable right = parseAssignment(tokens, rightIndex, env, mode, chainMatched);

        size_t leftIndex = i;
        Variable left = parseLogical(tokens, leftIndex, env, true, mode, chainMatched);

        if (left.getType() == Variable::REFERENCE)
        {
            if (mode == Mode::REFERENCE)
            {
                if (assignmentToken.getValue() == "=")
                {
                    if (right.getType() == Variable::REFERENCE)
                        *left.getReferenced() = right;
                    else
                        throw ParserException(ParserException::UNEXPECTED_SYNTAX, "reference variable must refers to a variable", assignmentToken.getLine(), assignmentToken.getCol());
                }
                else
                {
                    throw ParserException(ParserException::UNEXPECTED_SYNTAX, "reference variable can only be assigned with '=' syntax", assignmentToken.getLine(), assignmentToken.getCol());
                }
            }
            else
            {
                if (assignmentToken.getValue() == "=") {
                    *left.getReferenced() = right;
                }
                else if (assignmentToken.getValue() == "+=")
                    *left.getReferenced() = (*left.getReferenced()).add(right, assignmentToken);
                else if (assignmentToken.getValue() == "-=")
                    *left.getReferenced() = (*left.getReferenced()).subtract(right, assignmentToken);
                else if (assignmentToken.getValue() == "*=")
                    *left.getReferenced() = (*left.getReferenced()).mutliply(right, assignmentToken);
                else if (assignmentToken.getValue() == "/=")
                    *left.getReferenced() = (*left.getReferenced()).divide(right, assignmentToken);
            }
            i = rightIndex;
            return right;
        }

        throw ParserException(ParserException::INVALID_LVALUE, "cannot assign to invalid lvalue",
                              assignmentToken.getLine(), assignmentToken.getCol());
    }

    
    Variable value = parseLogical(tokens, i, env, false, mode, chainMatched);
    return value;
}

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

Variable parsePrimary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched)
{
    Token token = tokens[i++];

    if (mode == IF || mode == ELIF) {
        if (token.getType() == Token::BRACKETS && token.getValue() == "(") {
            
            if (chainMatched) {
                 i = tokens.size();
                 return Variable();
            }

            if (token.getChildren().empty()) {
                throw ParserException(ParserException::EXPECTED_EXPRESSION, "expression was expected", token.getLine(), token.getCol());
            }
            if (i + 1 > tokens.size()) {
                throw ParserException(ParserException::EXPECTED_EXPRESSION, "expression was expected after if statement", token.getLine(), token.getCol());
            }
            std::vector<Token> innerTokens = token.getChildren();
            
            bool dummyChain = false;
            Variable condition = parseExpression(innerTokens, env, Mode::NONE, dummyChain);
            
            if (condition.getType() != Variable::BOOLEAN) {
                throw ParserException(ParserException::UNEXPECTED_SYNTAX, "expression must have a bool type", token.getLine(), token.getCol());
            }
            
            if (!condition.getValue<bool>()) {
                i = tokens.size();
                return Variable();
            }
            
            chainMatched = true;

            if (tokens[i].getType() == Token::BRACKETS && tokens[i].getValue() == "{") {
                std::vector<Token> bodyTokens = tokens[i].getChildren();
                i++;
                if (i < tokens.size()) {
                    std::string msg;
                    msg = "unexpected syntax '";
                    msg += tokens[i].getValue();
                    msg += "'";
                    throw ParserException(ParserException::UNEXPECTED_SYNTAX, msg, tokens[i].getLine(), tokens[i].getCol());
                }
                return parse(bodyTokens, 2, env, {});
            }
            std::vector<Token> proceedTokens(tokens.begin() + i, tokens.end());
            i = tokens.size();
            return parse(proceedTokens, 2, env, {});
        } else {
            throw ParserException(ParserException::EXPECTED_SYNTAX, "'(' was expected", token.getLine(), token.getCol());
        }
    } else if (mode == ELSE) {
        if (token.getType() == Token::BRACKETS && token.getValue() == "{") {
            if (chainMatched) {
                i = tokens.size();
                return Variable();
            }
            
            chainMatched = true; 
            
            std::vector<Token> bodyTokens = token.getChildren();
            if (i < tokens.size()) {
                 std::string msg = "unexpected syntax '" + tokens[i].getValue() + "'";
                 throw ParserException(ParserException::UNEXPECTED_SYNTAX, msg, tokens[i].getLine(), tokens[i].getCol());
            }
            return parse(bodyTokens, 2, env, {});
        } else {
             throw ParserException(ParserException::EXPECTED_SYNTAX, "'{' was expected after else", token.getLine(), token.getCol());
        }
    } else if (mode == WHILE) {
        if (token.getType() == Token::BRACKETS && token.getValue() == "(") {
            std::vector<Token> conditionTokens = token.getChildren();
            
            std::vector<Token> bodyTokens;
            if (i < tokens.size() && tokens[i].getType() == Token::BRACKETS && tokens[i].getValue() == "{") {
                bodyTokens = tokens[i].getChildren();
                i++; 
            } else {
                 throw ParserException(ParserException::EXPECTED_SYNTAX, "'{' was expected after while", token.getLine(), token.getCol());
            }

            std::shared_ptr<Environment> loopEnv = std::make_shared<Environment>();
            loopEnv->parent = env;

            // Loop execution with BreakException handling
            while (true) {
                bool dummyChain = false;
                Variable condition = parseExpression(conditionTokens, env, Mode::NONE, dummyChain);
                
                if (condition.getType() != Variable::BOOLEAN) {
                    throw ParserException(ParserException::UNEXPECTED_SYNTAX, "expression must have a bool type", token.getLine(), token.getCol());
                }

                if (!condition.getValue<bool>()) {
                    break;
                }

                loopEnv->vars.clear();

                // Wrap execution in try-catch to handle break
                try {
                    parse(bodyTokens, -1, loopEnv, {});
                } catch (BreakException&) {
                    break; // Exit the loop immediately
                }
            }
            
            return Variable();
        } else {
            throw ParserException(ParserException::EXPECTED_SYNTAX, "'(' was expected", token.getLine(), token.getCol());
        }
    }

    switch (token.getType())
    {
    case Token::INTEGER:
        return Variable(std::stol(token.getValue()));
    case Token::DECIMAL:
        return Variable(std::stod(token.getValue()));
    case Token::STRING:
    case Token::FORMATTED_STRING:
        return Variable(token.getValue());
    case Token::CONSTANT:
        if (token.getValue() == "true" || token.getValue() == "false")
            return Variable(token.getValue() == "true");
        return Variable();
    case Token::NAME:
    {
        std::string name = token.getValue();

        bool isLValue = isLValueContext;

        if (i < tokens.size() && tokens[i].getType() == Token::ASSIGNMENT && tokens[i].getValue() == "=")
            isLValue = true;

        if (isLValue)
        {
            if (std::find(Parser_Def::keywords.begin(), Parser_Def::keywords.end(), name) != Parser_Def::keywords.end())
            {
                std::string msg;
                msg = "unexpected syntax '";
                msg += token.getValue();
                msg += "'";
                throw ParserException(ParserException::UNEXPECTED_SYNTAX, msg, token.getLine(), token.getCol());
            }
            Variable *found = env->find(name);
            if (!found)
            {
                env->vars[name] = Variable();
                found = &env->vars[name];
            }
            return Variable::makeRef(*found);
        }

        if (i < tokens.size())
        {
            if (tokens[i].getType() == Token::BRACKETS && tokens[i].getValue() == "(")
            {
                bool isBuiltin = false;
                if (!env->find(name))
                {
                    if (Functions::functions.find(name) != Functions::functions.end())
                        isBuiltin = true;
                    else
                    {
                        std::string msg;
                        msg = "name '";
                        msg += name;
                        msg += "' is undefined";
                        throw ParserException(ParserException::UNDEFINED_VARIABLE, msg, token.getLine(), token.getCol());
                    }
                }

                std::vector<std::vector<Token>> paramsRaw;
                std::vector<Token> temp;
                for (const auto &paramToken : tokens[i].getChildren())
                {
                    if ((paramToken.getType() != Token::OPERATOR || paramToken.getValue() != ","))
                        temp.push_back(paramToken);
                    else
                    {
                        if (temp.empty())
                        {
                            throw ParserException(ParserException::EXPECTED_EXPRESSION, "expression was expected", token.getLine(), token.getCol());
                        }
                        paramsRaw.push_back(temp);
                        temp.clear();
                    }
                }
                if (temp.size() > 0)
                {
                    paramsRaw.push_back(temp);
                }
                i++;
                if (!isBuiltin)
                {
                    if (env->find(name)->getRefType() != Variable::FUNCTION)
                        throw ParserException(ParserException::NOT_CALLABLE, "variable is not a function", token.getLine(), token.getCol());
                    
                    std::vector<std::string> params = env->find(name)->getValue<Function>().getParams();
                    std::unordered_map<std::string, Variable> arguments;

                    if (paramsRaw.size() != params.size())
                    {
                        std::string msg;
                        msg = "function requires ";
                        msg += std::to_string(params.size());
                        msg += " arguments, but ";
                        msg += std::to_string(paramsRaw.size());
                        msg += " was provided";
                        throw ParserException(ParserException::UNMATCH_ARGUMENT, msg, token.getLine(), token.getCol());
                    }
                    for (size_t i = 0; i < paramsRaw.size(); i++)
                        arguments[params[i]] = parseExpression(paramsRaw[i], env, mode, chainMatched);

                    return Variable(parse(env->find(name)->getValue<Function>().getBody(), 1, env, arguments));
                }
                std::vector<Variable> biArguments;
                for (auto param : paramsRaw)
                {
                    biArguments.push_back(parseExpression(param, env, mode, chainMatched));
                }
                return Variable(Functions::functions[name](biArguments, token));
            }
        }
        Variable *found = env->find(name);
        if (!found)
        {
            std::string msg;
            msg = "name '";
            msg += name;
            msg += "' is undefined";
            throw ParserException(ParserException::UNDEFINED_VARIABLE, msg, token.getLine(), token.getCol());
        }

        if (mode == Mode::REFERENCE)
            return Variable::makeRef(*found);

        return *found; // COPY instead of reference
    }

    case Token::BRACKETS:
        if (token.getValue() == "(")
            return parseExpression(token.getChildren(), env, mode, chainMatched);
        break;
    default:
        break;
    }
    std::string msg;
    msg = "unexpected syntax '";
    msg += token.getValue();
    msg += "'";
    throw ParserException(ParserException::UNEXPECTED_SYNTAX, msg, token.getLine(), token.getCol());
}


/**
 * parsingLevel:
 * 0 - global
 * 1 - function
 * 2 - if/elif/else
 * -1 - reuse parentEnv as local (for loops)
 */
Variable parse(const std::vector<Token>& tokens, int8_t parsingLevel, std::shared_ptr<Environment> parentEnv, const std::unordered_map<std::string, Variable>& extras)
{
    std::shared_ptr<Environment> localEnv;

    if (parsingLevel > 0)
    {
        localEnv = std::make_shared<Environment>();
        localEnv->parent = parentEnv ? parentEnv : globalEnv;

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
        localEnv = globalEnv;

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