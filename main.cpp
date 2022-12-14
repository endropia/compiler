#include <iostream>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "args.h"


int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "give me file plz";
        return 1;
    }
    std::ifstream reader(argv[1]);

    // -l - run lexer
    // -p - run parser

    if (!reader.good()) {
        std::cout << "file doesnt exist";
        return 1;
    }

    reader.close();

    if (check_arg(argc, argv, "-l")) {
        auto stream = std::ifstream(argv[1]);
        Lexer lexer(stream);
        while (true) {
            try {
                auto lexeme = lexer.GetLexeme();
                std::cout << lexeme << "\n";
                if (lexeme.GetType() == LexemeType::eof) {
                    break;
                }
            } catch (LexerException &err) {
                std::cout << err.what();
                break;
            }
        }
    }

    if (check_arg(argc, argv, "-p")) {
        auto stream = std::ifstream(argv[1]);
        Lexer lexer(stream);
        Parser parser(lexer);

        auto head = parser.Program();
//        for (auto i: head) {
//            i->DrawTree(std::cout, 1);
//        }
        head->DrawTree(std::cout, 1);
    }

    return 0;
}


