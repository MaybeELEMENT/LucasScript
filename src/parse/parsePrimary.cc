#include "parser.h"
#include <variable/function.h>
#include <builtin/functions/all.h>
extern Variable parseExpression(const std::vector<Token> &tokens, std::shared_ptr<Environment> env, Mode mode, bool &chainMatched);
extern Variable parse(const std::vector<Token>& tokens, int8_t parsingLevel = 1, std::shared_ptr<Environment> parentEnv = nullptr, const std::unordered_map<std::string, Variable>& extras = {});
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
                    
                    std::vector<FunctionArgument> params = env->find(name)->getValue<Function>().getParams();
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
                    for (size_t i = 0; i < paramsRaw.size(); i++) {
                        if(params[i].getIsRef())
                            arguments[params[i].getName()] = Variable::makeRef(*parseExpression(paramsRaw[i], env, REFERENCE, chainMatched).getReferenced());
                        else
                            arguments[params[i].getName()] = parseExpression(paramsRaw[i], env, mode, chainMatched);
                    }

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