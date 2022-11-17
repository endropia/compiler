#include "tester.h"
#include <iostream>


int main() {
    auto res = LexerTester("../tests/lexer").RunTests();
    res += ParserTester("../tests/parser").RunTests();

    std::cout << res;

    return 0;
}

