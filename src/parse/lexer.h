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
        CONSTANT
    };
private:
    Type type;
    std::string value;
    std::vector<Token> children;
    unsigned int line = 0;
    unsigned int column = 0;
public:
    unsigned int getLine() {return line;}
    unsigned int getCol() {return column;}
    std::string getValue() {return value;}
    std::vector<Token> getChildren() {return children;}
    Type getType() {return type;}
    Token(Type t, const std::string& v, std::vector<Token> c, unsigned int tline, unsigned int tcol) : type(t), value(v), children(c), line(tline), column(tcol) {}
    Token() {}
};
#include <exception>
#include <string>



// Custom exception class
class LexerException : public std::exception {
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
    unsigned int line = 0;
    unsigned int column = 0;
    explicit LexerException(LexerErrorType error, unsigned int eline, unsigned int ecolumn)
        : errorType(error), line(eline), column(ecolumn) {}
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

    void start();

    void dump();
};
#endif