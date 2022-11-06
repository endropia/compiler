#include "test.h"
#include "../lexer/lexer.h"
#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

bool Filter(const std::string &str) {
    return str.find(".in") != std::string::npos;
}

std::vector<std::string> FilesToVector() {
    std::vector<std::string> tests_files;
    std::string path = "../tests/lexer";
    for (const auto &entry: fs::directory_iterator(path)) {
        auto file_name = entry.path().string();
        if (Filter(file_name)) {
            tests_files.push_back(file_name.substr(0, file_name.size() - 3));
        }
    }
    return tests_files;
}


void CompareFiles(const std::vector<std::string> &files) {
    int success = 0, total = 0;
    std::ifstream file_out;

    for (auto &file: files) {
        Lexer lexer;

        total++;
        std::cout << file << "\n";
        file_out.open(file + ".out");
        lexer.OpenFile(file + ".in");
        bool test_success = true;

        if (!file_out.good()) {
            std::ofstream file_out_new;
            file_out_new.open(file + ".out");
            try {
                while (true) {
                    auto lexeme = lexer.GetLexeme();
                    file_out_new << lexeme << "\n";
                    file_out_new.flush();
                    if (lexeme.GetType() == LexemeType::eof) {
                        break;
                    }
                }
            } catch (LexerException err) {
                file_out_new << err.what();
            }
            continue;
        }

        while (true) {
            std::string str;
            std::getline(file_out, str);
            try {
                auto lexeme = lexer.GetLexeme();

                if (str == lexeme.String()) {
                    std::cout << "OK\t" << lexeme << "\n";
                } else {
                    test_success = false;
                    std::cout << "FAILED\n";
                    std::cout << "\tOut file: \t" << str << "\n";
                    std::cout << "\tLexer: \t\t" << lexeme.String() << "\n";
                }
                if (lexeme.GetType() == LexemeType::eof) {
                    break;
                }
            } catch (LexerException err) {
                if (str == err.what()) {
                    std::cout << "OK\t" << err.what() << "\n";
                } else {
                    test_success = false;
                    std::cout << "FAILED\n";
                    std::cout << "\tOut file: \t" << str << "\n";
                    std::cout << "\tLexer: \t\t" << err.what() << "\n";
                }
                break;
            }
        }

        success += test_success;
        file_out.close();
    }
    std::cout << "Successed tests:" << " " << success << " " << "from " << total << "\n";
    std::cout << "Failed tests:" << " " << total - success << "\n";
}

int main() {
    CompareFiles(FilesToVector());
    return 0;
}

