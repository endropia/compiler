#include <iostream>
#include <variant>

#include "lexer/lexer.h"

#include "tests/test.h"


std::string path_to_test = "../tests/lexer/02.in";

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "give me file plz";
        return 1;
    }
    std::ifstream reader(argv[1]);

    if (!reader.good()) {
        std::cout << "file doesnt exist";
    }

    reader.close();
    Lexer lexer;
    lexer.OpenFile(argv[1]);

    while (true) {
        try {
            auto lexeme = lexer.GetLexeme();
            std::cout << lexeme << "\n";
            if (lexeme.GetType() == LexemeType::eof) {
                break;
            }
        } catch (LexerException err) {
            std::cout << err.what();
            break;
        }
    }

    return 0;
}


