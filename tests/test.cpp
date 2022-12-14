#include "tester.h"
#include "../args.h"
#include <iostream>


int main(int argc, char **argv) {
    TestResult res;
    if (check_arg(argc, argv, "-l")) {
        res += LexerTester("../tests/lexer").RunTests();
    }
    if (check_arg(argc, argv, "-p")) {
        res += ParserTester("../tests/parser").RunTests();
    }
    std::cout << res;
    return 0;
}

