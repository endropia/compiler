#ifndef COMPILER_TESTER_H
#define COMPILER_TESTER_H

#include <iostream>
#include <vector>

class TestResult {
public:
    TestResult() : counter_all(0), counter_failed(0) {}

    TestResult &operator+=(const TestResult &res);

    friend std::ostream &operator<<(std::ostream &os, const TestResult &res);

    void failed();

    void success();

private:
    int counter_all, counter_failed;

    TestResult(int counter_all, int counter_failed) : counter_all(counter_all), counter_failed(counter_failed) {}
};

class Tester {
public:
    Tester(std::string &path) : path(path) {}

    TestResult RunTests();

protected:
    std::string path;

    virtual bool RunTest(const std::string &file) = 0;

    std::vector<std::string> FilesToVector();
};

class LexerTester : public Tester {
public:
    explicit LexerTester(std::string path) : Tester(path) {}

    bool RunTest(const std::string &file) override;
};

class ParserTester : public Tester {
public:
    explicit ParserTester(std::string path) : Tester(path) {}

    bool RunTest(const std::string &file) override;
};

#endif //COMPILER_TESTER_H
