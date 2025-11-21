// By Lucas
#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
class Token {
public:
    enum Type {
        NONE,
        KEYWORD,
        ASSIGNMENT,
        COMPARISON,
        INTEGER,
        DECIMAL,
        OPERATOR,
        END_OF_LINE,
        BRACKETS,
        STRING,
        FORMATTED_STRING,
        CONSTANT,
        NAME
    };
private:
    Type type;
    std::string value;
    std::vector<Token> children;
    unsigned int line = 0;
    unsigned int column = 0;
public:
    unsigned int getLine() const {return line;}
    unsigned int getCol() const {return column;}
    std::string getValue() const {return value;}
    const std::vector<Token> getChildren() const {return children;}
    Type getType() const {return type;}
    Token(Type t, const std::string& v, const std::vector<Token> c, unsigned int tline, unsigned int tcol) : type(t), value(v), children(c), line(tline), column(tcol) {}
    Token() {}
};



// Custom exception class
class LexerException{
public:
    enum LexerErrorType {
        UNTERMINATED_STRING,
        INVALID_CHARACTER,
        UNKNOWN_ERROR,
        UNSUPPORTED_ESCAPE_SEQUENCE,
        UNTERMINATED_ESCAPE_SEQUENCE,
        EXPECTED_ESCAPE_CHARACTER,
        UNEXPECTED_ESCAPE_SEQUENCE,
        INVALID_SYNTAX,
        UNEXPECTED_SYNTAX,
        EXPECTED_CLOSING,
        CLOSING_UNMATCH
    };
    LexerErrorType errorType;
    std::string msg;
    unsigned int line = 0;
    unsigned int column = 0;
    std::string errorCode() {
        switch(errorType) {
            case UNTERMINATED_STRING:
                return "UNTERMINATED_STRING";
            case INVALID_CHARACTER:
                return "INVALID_CHARACTER";
            case UNSUPPORTED_ESCAPE_SEQUENCE:
                return "UNSUPPORTED_ESCAPE_SEQUENCE";
            case UNTERMINATED_ESCAPE_SEQUENCE:
                return "UNTERMINATED_ESCAPE_SEQUENCE";
            case EXPECTED_ESCAPE_CHARACTER:
                return "EXPECTED_ESCAPE_CHARACTER";
            case UNEXPECTED_ESCAPE_SEQUENCE:
                return "UNEXPECTED_ESCAPE_SEQUENCE";
            case INVALID_SYNTAX:
                return "INVALID_SYNTAX";
            case UNEXPECTED_SYNTAX:
                return "UNEXPECTED_SYNTAX";
            case EXPECTED_CLOSING:
                return "EXPECTED_CLOSING";
            case CLOSING_UNMATCH:
                return "CLOSING_UNMATCH";
            default:
                return "UNKNOWN_ERROR";
        }
    }
    LexerException(LexerErrorType error, std::string message, unsigned int eline, unsigned int ecolumn)
        : errorType(error), msg(message), line(eline), column(ecolumn) {}
};
class Lexer {
    std::string src;
    std::vector<Token> tokens;
    unsigned int index = 0;
    unsigned int line = 1;
    unsigned int column = 1;
    int layer = 0;
    char inString = 0; //0-no, 1-single, 2-double, 3-formatted string
    
    std::vector<Token> parse(char endChar = ' ');
    void addToken(std::string& token, Token::Type& type, std::vector<Token>& res, std::vector<Token> children = {});
public:
    Lexer(const std::string& input) : src(input) {}

    std::vector<Token> getTokens();

    void start();

    void dump();
};
#endif