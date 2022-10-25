#include <iostream>

#include "lexer/lexer.h"

std::string path_to_test = "../tests/lexer/01.in";

int main() {
//    std::ifstream f(path_to_test);
//
//    char c;
//    auto i = 3;
//    while (i--) {
//        c = f.get();
//        std::cout << c <<  " " << f.tellg() << "\n";
//    }
//    std::cout << "AFTER UNGET: \n";
//    f.unget();
//    f.unget();f.unget();
//    i = 3;
//    while (i--) {
//        c = f.get();
//        std::cout << c << f.tellg() << "\n";
//    }
    std::ios_base::sync_with_stdio(false);
    Lexer lexer;
    try {
        for (const auto &item: lexer.Scan(path_to_test)) {
            std::cout << item << "\n";
        }
    } catch (...) {
        std::cout << "ERROR";
    }
    return 0;
}
