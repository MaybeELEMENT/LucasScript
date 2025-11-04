// By Lucas
#include "lexer.h"

void Lexer::addToken(std::string& token, Token::Type& type, std::vector<Token>& tokens, std::vector<Token> children) {
    if(type == Token::KEYWORD) {
        if(token == "true" || token == "false" || token == "null") {
            type = Token::CONSTANT;
        }
    }
    tokens.push_back(Token(type, token, children, line, column));
    token = "";
    type = Token::NONE;
    inString = 0;
}

std::string getToken(Token::Type type) {
    switch(type) {
        case Token::KEYWORD: return "KEYWORD";
        case Token::ASSIGNMENT: return "ASSIGNMENT";
        case Token::COMPARISON: return "COMPARISON";
        case Token::INTEGER: return "INTEGER";
        case Token::DECIMAL: return "DECIMAL";
        case Token::OPERATOR: return "OPERATOR";
        case Token::END_OF_LINE: return "END_OF_LINE";
        case Token::BRACKETS: return "BRACKETS";
        case Token::STRING: return "STRING";
        case Token::FORMATTED_STRING: return "FORMATTED_STRING";
        case Token::CONSTANT: return "CONSTANT";
        default: return "NONE";
    }
}

void dumping(const std::vector<Token>& tokens, int depth) {
    std::string indent(depth * 4, ' ');
    for(auto token : tokens) {
        if(token.getType() == Token::END_OF_LINE) {
            printf("END_OF_LINE\n");
            continue;
        }
        printf("%s%s, '%s', %d, %d\n", indent.c_str(), getToken(token.getType()).c_str(), token.getValue().c_str(), token.getLine(), token.getCol());
        if(token.getType() == Token::BRACKETS) {
            dumping(token.getChildren(), depth + 1);
        }
    }
}

void Lexer::dump() {
    dumping(tokens, 0);
    // printf("Src: %s\n\n", src.c_str());
    // for(auto token : tokens) {
    //     if(token.getType() == Token::END_OF_LINE) {
    //         printf("END_OF_LINE\n");
    //         continue;
    //     }
    //     printf("%s, '%s', %i\n", getToken(token.getType()).c_str(), token.getValue().c_str(), token.getLine());
    // }
}

std::vector<Token> Lexer::parse(char endChar) {
    std::vector<Token> res;
    std::string currentToken;
    Token::Type type = Token::NONE;
    

    for(; index < src.length(); index++, column++) {
        char c = src[index];
        switch(c) {
            case '#':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                while(index < src.length() && src[index] != '\n') {
                    index++;
                }
                index--;
                break;
            case '\n':
                if(type != Token::NONE) {
                    if(inString != 0) {
                        throw LexerException(LexerException::UNTERMINATED_STRING, line, column);
                    }
                    addToken(currentToken, type, res);
                }
                res.push_back(Token(Token::END_OF_LINE, "\n", {}, line, column));
                line++;
                column = 0;
                break;
            case ' ':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                break;
            case '\'':
                if(type != Token::NONE && inString == 0) {
                    addToken(currentToken, type, res);
                }
                if(inString == 2 || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(inString == 1) {
                    inString = 0;
                    addToken(currentToken, type, res);
                    break;
                }
                inString = 1;
                type = Token::STRING;
                break;
            case '`':
                if(type != Token::NONE && inString == 0) {
                    addToken(currentToken, type, res);
                }
                if(type == Token::STRING) {
                    currentToken += c;
                    break;
                }
                if(inString == 3) {
                    inString = 0;
                    addToken(currentToken, type, res);
                    break;
                }
                inString = 3;
                type = Token::FORMATTED_STRING;
                break;
            case '"':
                if(type != Token::NONE && inString == 0) {
                    addToken(currentToken, type, res);
                }
                if(inString == 1 || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(inString == 2) {
                    inString = 0;
                    addToken(currentToken, type, res);
                    break;
                }
                inString = 2;
                type = Token::STRING;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type == Token::KEYWORD || type == Token::INTEGER || type == Token::DECIMAL || (type == Token::OPERATOR && currentToken == ".")) {
                    if(type == Token::OPERATOR && currentToken == ".") {
                        type = Token::DECIMAL;
                    }
                    currentToken += c;
                } else {
                    if(type != Token::NONE) {
                        addToken(currentToken, type, res);
                    }
                    type = Token::INTEGER;
                    currentToken += c;
                }
                break;
            case '\\':
            {
                if(type != Token::STRING && type != Token::FORMATTED_STRING) {
                    throw LexerException(LexerException::UNEXPECTED_ESCAPE_SEQUENCE, line, column);
                }
                if(index + 1 >= src.length()) {
                    throw LexerException(LexerException::UNTERMINATED_ESCAPE_SEQUENCE, line, column);
                }
                char nextChar = src[++index];
                switch(nextChar) {
                    case 'n':
                        currentToken += '\n';
                        break;
                    case 't':
                        currentToken += '\t';
                        break;
                    case 'f':
                        currentToken += '\f';
                        break;
                    case 'r':
                        currentToken += '\r';
                        break;
                    case 'b':
                        currentToken += '\b';
                        break;
                    case 'a':
                        currentToken += '\a';
                        break;
                    case '\'':
                        currentToken += '\'';
                        break;
                    case '"':
                        currentToken += '"';
                        break;
                    case '`':
                        currentToken += '`';
                        break;
                    case '\\':
                        currentToken += '\\';
                    case '\n':
                    case '\t':
                    case '\f':
                    case '\r':
                    case '\a':
                    case '\b':
                        throw LexerException(LexerException::EXPECTED_ESCAPE_CHARACTER, line, column);
                    default:
                        throw LexerException(LexerException::UNSUPPORTED_ESCAPE_SEQUENCE, line, column);
                }
                break;
            }
            case '.':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type == Token::INTEGER) {
                    currentToken += c;
                    type = Token::DECIMAL;
                    break;
                }
                if(type == Token::DECIMAL) {
                    throw LexerException(LexerException::INVALID_SYNTAX, line, column);
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                type = Token::OPERATOR;
                currentToken += c;
                break;
            
            case '=':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=') {
                    index++;
                    currentToken += src[index];
                    type = Token::COMPARISON;
                    break;
                }
                type = Token::ASSIGNMENT;
                break;
            case '!':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=') {
                    index++;
                    currentToken += src[index];
                    type = Token::COMPARISON;
                    break;
                }
                type = Token::COMPARISON;
                break;
            case '%':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                type = Token::OPERATOR;
                break;
            case '>':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=') {
                    index++;
                    currentToken += src[index];
                    type = Token::COMPARISON;
                    break;
                }
                type = Token::COMPARISON;
                break;
            case '<':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=') {
                    index++;
                    currentToken += src[index];
                    type = Token::COMPARISON;
                    break;
                }
                type = Token::COMPARISON;
                break;
            
            case '+':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=' || src[index + 1] == '+') {
                    index++;
                    currentToken += src[index];
                    type = Token::ASSIGNMENT;
                    break;
                }
                type = Token::OPERATOR;
                break;
            case '-':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=' || src[index + 1] == '-') {
                    index++;
                    currentToken += src[index];
                    type = Token::ASSIGNMENT;
                    break;
                }
                type = Token::OPERATOR;
                break;
            case '*':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=') {
                    index++;
                    currentToken += src[index];
                    type = Token::ASSIGNMENT;
                    break;
                }
                if(src[index + 1] == '*') {
                    index++;
                    currentToken += src[index];
                }
                type = Token::OPERATOR;
                break;
            case '/':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                if(src[index + 1] == '=') {
                    index++;
                    currentToken += src[index];
                    type = Token::ASSIGNMENT;
                    break;
                }
                if(src[index + 1] == '/') {
                    index++;
                    currentToken += src[index];
                }
                type = Token::OPERATOR;
                break;
            case '(':
            case '{':
            case '[':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                layer++;
                currentToken += c;
                type = Token::BRACKETS;
                index++;
                addToken(currentToken, type, res, parse(c == '(' ? ')' : c == '{' ? '}' : ']'));
                break;
            case ')':
            case '}':
            case ']':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                if(layer < 1) {
                    throw LexerException(LexerException::UNEXPECTED_SYNTAX, line, column);
                }
                if(c != endChar) {
                    throw LexerException(LexerException::CLOSING_UNMATCH, line, column);
                }
                layer--;
                return res;
            case ',':
            case ':':
            case '?':
                if(type == Token::STRING || type == Token::FORMATTED_STRING) {
                    currentToken += c;
                    break;
                }
                if(type != Token::NONE) {
                    addToken(currentToken, type, res);
                }
                currentToken += c;
                type = Token::OPERATOR;
                break;
            default:
                if(type == Token::NONE)
                    type = Token::KEYWORD;
                if(type != Token::KEYWORD && type != Token::STRING && type != Token::FORMATTED_STRING) {
                    addToken(currentToken, type, res);
                    type = Token::KEYWORD;
                }
                currentToken += c;
                break;
        }
        // Additional parsing logic would be implemented here
    }
    
    if(inString != 0) {
        throw LexerException(LexerException::UNTERMINATED_STRING, line, column);
    }
    if(type != Token::NONE) { 
        addToken(currentToken, type, res);
    }
    if(layer > 0 && index >= src.length()) {
        throw LexerException(LexerException::EXPECTED_CLOSING, line, column);
    }
    return res;
}

void Lexer::start() {
    tokens = parse();
}