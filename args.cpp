#include "args.h"
#include <string.h>

bool CheckArg(int argc, char **argv, const std::string &arg) {
    for (auto i = 1; i < argc; ++i) {
        if (strcmp(argv[i], arg.c_str()) == 0) {
            return true;
        }
    }
    return false;
}
