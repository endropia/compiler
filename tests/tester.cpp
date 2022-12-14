#include <filesystem>
#include <fstream>
#include "tester.h"

#include "../lexer/lexer.h"
#include "../parser/parser.h"

TestResult &TestResult::operator+=(const TestResult &res) {
    counter_all += res.counter_all;
    counter_failed += res.counter_failed;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const TestResult &res) {
    os << "Tests: " << res.counter_all << "; Failed: " << res.counter_failed;
    return os;
}

void TestResult::failed() {
    counter_all++;
    counter_failed++;
}

void TestResult::success() {
    counter_all++;
}

TestResult Tester::RunTests() {
    TestResult res;
    auto files = FilesToVector();

    for (const auto &file: files) {
        std::cout << file << "\n";
        if (RunTest(file)) {
            res.success();
        } else {
            res.failed();
        }
    }
    return res;
}

std::vector<std::string> Tester::FilesToVector() {
    std::vector<std::string> tests_files;
    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        auto file_name = entry.path().string();
        if (file_name.find(".in") != std::string::npos) {
            tests_files.push_back(file_name.substr(0, file_name.size() - 3));
        }
    }
    return tests_files;
}

bool LexerTester::RunTest(const std::string &file) {
    auto stream = std::ifstream(file + ".in");
    Lexer lexer(stream);

    std::ifstream file_out(file + ".out");

    bool is_success = true;

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
        } catch (LexerException &err) {
            file_out_new << err.what();
        }
        return is_success;
    }

    while (true) {
        std::string str;
        std::getline(file_out, str);
        try {
            auto lexeme = lexer.GetLexeme();

            if (str == lexeme.String()) {
                std::cout << "OK\t" << lexeme << "\n";
            } else {
                is_success = false;
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
                is_success = false;
                std::cout << "FAILED\n";
                std::cout << "\tOut file: \t" << str << "\n";
                std::cout << "\tLexer: \t\t" << err.what() << "\n";
            }
            break;
        }
    }
    file_out.close();

    return is_success;
}

std::string ReadFile(const std::string &path) {
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

bool ParserTester::RunTest(const std::string &file) {
    auto stream = std::ifstream(file + ".in");
    Lexer lexer(stream);
    Parser parser(lexer);

    std::ifstream file_out(file + ".out");

    bool is_success = true;

    if (!file_out.good()) {
        std::ofstream file_out_new;
        file_out_new.open(file + ".out");
        try {
            std::stringstream parser_answer;
            parser.Expression()->DrawTree(parser_answer, 1);
            file_out_new << parser_answer.str();
        } catch (ParserException &err) {
            file_out_new << err.what();
        }
        return is_success;
    }
    file_out.close();

    auto out_file_content = ReadFile(file + ".out");
    try {
        std::stringstream parser_answer;
        parser.Program()->DrawTree(parser_answer, 1);

        if (parser_answer.str() == out_file_content) {
            std::cout << "OK\n";
        } else {
            is_success = false;
            std::cout << "FAILED\n";
            std::cout << "Out file: \n" << out_file_content << "\n";
            std::cout << "Parser: \n" << parser_answer.str() << "\n";
        }
    } catch (ParserException &err) {
        if (out_file_content == err.what()) {
            std::cout << "OK\n";
        } else {
            is_success = false;
            std::cout << "FAILED\n";
            std::cout << "Out file: \n" << out_file_content << "\n";
            std::cout << "Lexer: \n" << err.what() << "\n";
        }
    }

    return is_success;
}
