#include "tester.h"
#include "../args.h"
#include <iostream>


int main(int argc, char **argv) {
    TestResult res;
    if (CheckArg(argc, argv, "-l")) {
        res += LexerTester("../tests/lexer").RunTests();
    }
    if (CheckArg(argc, argv, "-p")) {
        res += ParserTester("../tests/parser").RunTests();
    }
    if (CheckArg(argc, argv, "-s")) {
        res += SemanticTester("../tests/semantic").RunTests();
    }
    std::cout << res;
    return 0;
}

