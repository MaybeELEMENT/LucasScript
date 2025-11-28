// Created on 30 Oct 2025 by Lucas & Skript
#include <stdio.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <parse/lexer.h>
#include <parse/parser.h>
#include <sstream>
#include <iomanip>
#include <utils/argument.h>
extern std::unordered_map<std::string, Variable> varGlobal;

int main(int argc, char** argv)
{
    Lucas::ArgumentParser argparser;
    argparser.parse(argc, argv);
    // Everything starts here
    if(argparser.getRaw().size() < 1) {
        printf("No input file provided.\n");
        return 1;
    }
    std::ifstream file(argparser.getRaw()[0]);
    std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    try
    {
        Lexer lexer(src);
        lexer.start();
        Parser parser(lexer.getTokens());
        parser.start();

        // lexer.dump();
    }
    catch (LexerException &e)
    {
        std::istringstream stream(src);
        std::string line;

        printf("\nError occurred on line %d, column %d:\n", e.line, e.column);

        int lineNum = 0;

        // compute width dynamically from largest line number
        int width = 0;
        {
            int temp = e.line;
            while (temp > 0)
            {
                width++;
                temp /= 10;
            }
        }

        while (std::getline(stream, line))
        {
            lineNum++;
            if (lineNum >= e.line - 1 && lineNum <= e.line)
            {
                printf("\t%*d | %s\n", width, lineNum, line.c_str());
                if (lineNum >= e.line)
                    break;
            }
        }

        std::string pointer(e.column, '-');
        pointer.back() = '^';
        printf("\t%*s--%s\n\t%s: %s\n", width, "", pointer.c_str(),
               e.errorCode().c_str(), e.msg.c_str());
        return 1;
    }
    catch (ParserException &e)
    {
        std::istringstream stream(src);
        std::string line;

        printf("\nError occurred on line %d, column %d:\n", e.line, e.column);

        int lineNum = 0;
        // compute width dynamically from largest line number
        int width = 0;
        {
            int temp = e.line;
            while (temp > 0)
            {
                width++;
                temp /= 10;
            }
        }

        while (std::getline(stream, line))
        {
            lineNum++;
            if (lineNum >= e.line - 1 && lineNum <= e.line)
            {
                printf("\t%*d | %s\n", width, lineNum, line.c_str());
                if (lineNum >= e.line)
                    break;
            }
        }

        std::string pointer(e.column, '-');
        pointer.back() = '^';
        printf("\t%*s--%s\n\t%s: %s\n", width, "", pointer.c_str(),
               e.errorCode().c_str(), e.msg.c_str());
        return 1;
    }

    return 0;
}