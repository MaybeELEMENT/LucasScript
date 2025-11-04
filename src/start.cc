// Created on 30 Oct 2025 by Lucas & Skript
#include <stdio.h>
#include <string>
#include <fstream>
#include "parse/lexer.h"
int main() {
    // Everything starts here
    std::ifstream file("app.lex");
    std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    try {
        Lexer lexer(src);
        lexer.start();
        lexer.dump();
    }
    catch(LexerException& e) {
        printf("Lexer Error on line %d, column %d: %d\n", e.line, e.column, e.errorType);
        return 1;
    }
    
    
    return 0;
}