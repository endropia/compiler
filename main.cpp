#include <iostream>

#include "lexer/lexer.h"

#include "tests/test.h"


std::string path_to_test = "../tests/lexer/02.in";

int main() {

//    std::ios_base::sync_with_stdio(false);
//    Lexer lexer;
//    try {
//        for (const auto &item: lexer.Scan(path_to_test)) {
//            std::cout << item << "\n";
//        }
//    } catch (...) {
//        std::cout << "ERROR";
//    }
//
//    for (auto &item: FilesToVector()) {
//        std::cout << item << "\n";
//    }

    CompareFiles(FilesToVector());
    return 0;
}
