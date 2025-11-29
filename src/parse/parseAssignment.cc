#include "parser.h"

extern Variable parseLogical(const std::vector<Token> &tokens, size_t &i, std::shared_ptr<Environment> env, bool isLValueContext, Mode mode, bool &chainMatched);


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
                std::vector<FunctionArgument> params;

                if (tokens[i + 1].getType() == Token::BRACKETS &&
                    tokens[i + 1].getValue() == "(")
                {
                    bool comma = false;
                    bool isRef = false;
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
                        if(paramToken.getType() == Token::KEYWORD && paramToken.getValue() == "ref" && !isRef) {
                                isRef = true;
                                continue;
                        } 
                        if (paramToken.getType() == Token::NAME) {
                            params.push_back(FunctionArgument(paramToken.getValue(), isRef));
                            isRef = false;
                            comma = true;
                            continue;
                        }
                        
                        std::string msg;
                            msg = "unexpected syntax '";
                            msg += paramToken.getValue();
                            msg += "'";
                            throw ParserException(ParserException::UNEXPECTED_SYNTAX, msg,
                                                      paramToken.getLine(), paramToken.getCol());
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