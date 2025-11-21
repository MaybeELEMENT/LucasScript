#include "parser.h"
#include <unordered_map>
#include <variable/function.h>
#include <builtin/functions/all.h>

std::shared_ptr<Environment> globalEnv = std::make_shared<Environment>();

enum Mode
{
    NONE,
    REFERENCE,
    RETURN,
    IF,
    ELIF,
    ELSE,
};

// Declare function
Variable parse(std::vector<Token> tokens, int8_t parsingLevel = 1, std::unordered_map<std::string, Variable> extras = {});
Variable parseMultiplicative(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode);
Variable parseAdditive(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode);
Variable parseRelational(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode);
Variable parseEquality(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode);
Variable parseLogical(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode);
Variable parseAssignment(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, Mode mode);
Variable parsePrimary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode);
Variable parseExpression(const std::vector<Token> &tokens, std::shared_ptr<Environment> env, Mode mode);

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

Variable parseMultiplicative(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode)
{
    Variable left = parsePrimary(tokens, i, env, isLValueContext, mode);

    while (i < tokens.size())
    {
        Token token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::OPERATOR ||
            (token.getValue() != "*" && token.getValue() != "/"))
            break;

        i++;
        Variable right = parsePrimary(tokens, i, env, isLValueContext, mode);

        if (token.getValue() == "*")
            left = left.mutliply(right, token);
        else
            left = left.divide(right, token);
    }

    return left;
}

Variable parseAdditive(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode)
{
    Variable left = parseMultiplicative(tokens, i, env, isLValueContext, mode);

    while (i < tokens.size())
    {
        Token token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::OPERATOR ||
            (token.getValue() != "+" && token.getValue() != "-"))
            break;

        i++;
        Variable right = parseMultiplicative(tokens, i, env, isLValueContext, mode);

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

Variable parseRelational(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode)
{
    Variable left = parseAdditive(tokens, i, env, isLValueContext, mode);

    while (i < tokens.size())
    {
        Token token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::COMPARISON ||
            (token.getValue() != "<" && token.getValue() != ">" &&
             token.getValue() != "<=" && token.getValue() != ">="))
            break;

        i++;
        Variable right = parseAdditive(tokens, i, env, isLValueContext, mode);

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

Variable parseEquality(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode)
{
    Variable left = parseRelational(tokens, i, env, isLValueContext, mode);

    while (i < tokens.size())
    {
        Token token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::COMPARISON ||
            (token.getValue() != "==" && token.getValue() != "!="))
            break;

        i++;
        Variable right = parseRelational(tokens, i, env, isLValueContext, mode);

        if (token.getValue() == "==")
            left = (left.compareEqual(right, token));
        else
            left = (left.compareInequal(right, token));
    }

    return left;
}

Variable parseLogical(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode)
{
    Variable left = parseEquality(tokens, i, env, isLValueContext, mode);

    while (i < tokens.size())
    {
        Token token = tokens[i];
        if (token.getType() == Token::ASSIGNMENT)
            break;
        if (token.getType() != Token::KEYWORD || (token.getValue() != "and" && token.getValue() == "or"))
            break;

        i++;

        Variable right = parseEquality(tokens, i, env, isLValueContext, mode);

        if (token.getValue() == "and")
            left = (left.compareAnd(right, token));
        else
            left = (left.compareOr(right, token));
    }
    
    return left;
}

Variable parseAssignment(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, Mode mode)
{
    size_t assignIndex = SIZE_MAX;
    Token assignmentToken;

    // Find '=' token first to decide if this is an assignment expression
    for (size_t j = i; j < tokens.size(); ++j)
    {
        if (tokens[j].getType() == Token::ASSIGNMENT)
        {
            assignmentToken = tokens[j];
            assignIndex = j;
            break;
        }
    }

    // Case 1: This is an assignment
    if (assignIndex != SIZE_MAX)
    {
        if (tokens[assignIndex + 1].getType() == Token::BRACKETS &&
            tokens[assignIndex + 1].getValue() == "{")
        {
            // Left must be NAME + "(" + arguments + ")", otherwise not function
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
                // Right side is function body
                std::vector<Token> body = tokens[assignIndex + 1].getChildren();

                auto func = Function(body, params);
                if (assignmentToken.getValue() != "=")
                    throw ParserException(ParserException::UNEXPECTED_SYNTAX, "function can only be defined with '=' assignment", assignmentToken.getLine(), assignmentToken.getCol());

                env->vars[funcName] = Variable(func);
                i = assignIndex + 2; // assignIndex + 1 = { ... } token
                
                return Variable();
            }
        }

        size_t rightStart = assignIndex + 1;
        size_t rightIndex = rightStart;
        
        Variable right = parseAssignment(tokens, rightIndex, env, mode);

        size_t leftIndex = i;
        Variable left = parseLogical(tokens, leftIndex, env, true, mode);

        if (left.getType() == Variable::REFERENCE)
        {
            if (mode == Mode::REFERENCE)
            {
                if (assignmentToken.getValue() == "=")
                {
                    // If right is itself a reference, just re-point left to that target
                    // if(right.getRefType() == Variable::NULL_TYPE) {
                    // }
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
                if (assignmentToken.getValue() == "=")
                    *left.getReferenced() = right;
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

    
    Variable value = parseLogical(tokens, i, env, false, mode);
    return value;
}

Variable parseExpression(const std::vector<Token> &tokens, std::shared_ptr<Environment> env, Mode mode)
{
    
    if (tokens.empty()) {
        return Variable();
    }
    size_t i = 0;
    Variable res = parseAssignment(tokens, i, env, mode);

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

Variable parsePrimary(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode)
{
    Token token = tokens[i++];

    if(mode == IF || mode == ELIF) {
        if(token.getType() == Token::BRACKETS && token.getValue() == "(") {
            if(token.getChildren().empty()) {
                throw ParserException(ParserException::EXPECTED_EXPRESSION, "expression was expected", token.getLine(), token.getCol());
            }
            if(i + 1 > tokens.size()) {
                throw ParserException(ParserException::EXPECTED_EXPRESSION, "expression was expected after if statement", token.getLine(), token.getCol());
            }
            std::vector<Token> innerTokens = token.getChildren();
            Variable condition = parseExpression(innerTokens, env, Mode::NONE);
            if(condition.getType() != Variable::BOOLEAN) {
                throw ParserException(ParserException::UNEXPECTED_SYNTAX, "expression must have a bool type", token.getLine(), token.getCol());
            }
            if(!condition.getValue<bool>()) {
                i = tokens.size();
                return Variable();
            }
            if(tokens[i].getType() == Token::BRACKETS && tokens[i].getValue() == "{") {
                std::vector<Token> bodyTokens = tokens[i].getChildren();
                i++;
                if(i < tokens.size()) {
                    std::string msg;
                    msg = "unexpected syntax '";
                    msg += tokens[i].getValue();
                    msg += "'";
                    throw ParserException(ParserException::EXPECTED_SYNTAX, msg, tokens[i].getLine(), tokens[i].getCol());
                }
                return parse(bodyTokens, 2, env->vars);
            }
            std::vector<Token> proceedTokens(tokens.begin() + i, tokens.end());
            i = tokens.size();
            return parse(proceedTokens, 2, env->vars);
            
            // i--; // step back to re-process the brackets as part of if/elif/else
            // return condition;
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

        // left side: create variable if not exist
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

        // right side: variable must exist, if so then return a copy
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
                        arguments[params[i]] = parseExpression(paramsRaw[i], env, mode);

                    return Variable(parse(env->find(name)->getValue<Function>().getBody(), 1, arguments));
                }
                std::vector<Variable> biArguments;
                for (auto param : paramsRaw)
                {
                    biArguments.push_back(parseExpression(param, env, mode));
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
            return parseExpression(token.getChildren(), env, mode);
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
 */
Variable parse(std::vector<Token> tokens, int8_t parsingLevel, const std::unordered_map<std::string, Variable> extras)
{
    std::shared_ptr<Environment> localEnv;

    if (parsingLevel > 0)
    {
        localEnv = std::make_shared<Environment>();
        localEnv->parent = globalEnv;

        // Add extras to localEnv, but don't touch globalEnv
        for (const auto &[key, val] : extras)
            localEnv->vars[key] = val; // shadows global variables with same name
    }
    else
        localEnv = globalEnv;

    if (tokens.empty())
        return Variable(); // return Null

    std::vector<Token> expression;
    Mode mode = Mode::NONE;
    Mode lastMode = Mode::NONE;

    for (auto token : tokens)
    {
        switch (token.getType())
        {
        case Token::END_OF_LINE:
        case Token::OPERATOR:
            if ((token.getType() == Token::OPERATOR && token.getValue() == ";") || token.getType() == Token::END_OF_LINE)
            {
                if(mode == RETURN)
                    return parseExpression(expression, localEnv, mode);
                parseExpression(expression, localEnv, mode);
                expression.clear();
                mode = Mode::NONE;
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
                    lastMode = mode;
                }
                else if (token.getValue() == "elif") {
                    mode = Mode::ELIF;
                    if(lastMode != Mode::IF)
                        throw ParserException(ParserException::UNEXPECTED_SYNTAX, "'elif' without preceding if statement", token.getLine(), token.getCol());
                    lastMode = mode; 
                }
                else if (token.getValue() == "else") {
                    mode = Mode::ELSE;
                    if(lastMode != Mode::IF && lastMode != Mode::ELIF)
                        throw ParserException(ParserException::UNEXPECTED_SYNTAX, "'else' without preceding if or elif statement", token.getLine(), token.getCol());
                    lastMode = mode;
                }
                break;
            }
            expression.push_back(token);
            break;
        }
    }
    return parseExpression(expression, localEnv, mode);
}