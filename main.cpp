#include <iostream>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "args.h"
#include "semantic/semantic.h"


int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "give me file plz";
        return 1;
    }
    std::ifstream reader(argv[1]);

    // -l - run lexer
    // -p - run parser
    // -s - run semantic

    if (!reader.good()) {
        std::cout << "file doesnt exist";
        return 1;
    }

    reader.close();

    if (CheckArg(argc, argv, "-l")) {
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

    if (CheckArg(argc, argv, "-p")) {
        auto stream = std::ifstream(argv[1]);
        Lexer lexer(stream);
        Parser parser(lexer);

        auto head = parser.Program();
        head->DrawTree(std::cout, 1);
    }

    if (CheckArg(argc, argv, "-s")) {
        auto stream = std::ifstream(argv[1]);
        Lexer lexer(stream);
        Parser parser(lexer);

        auto head = parser.Program();
        auto semantic_visitor = new Semantic();
        head->Accept(semantic_visitor);
        head->DrawTree(std::cout, 1);
        std::cout << "\n";
        semantic_visitor->GetStack().Draw(std::cout);
    }

    return 0;
}


