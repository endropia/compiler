#ifndef CMPILER_TEST_H
#define COMPILER_TEST_H

#include <string>
#include <vector>



std::vector<std::string> FilesToVector();

void CompareFiles(const std::vector<std::string>& files);
#endif // COMPILER_TEST_H