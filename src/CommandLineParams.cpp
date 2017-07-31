#include "CommandLineParams.h"

#include <vector>

namespace {
    std::vector<std::string> params;
}

void initCLParams(int argc, char *argv[]) {
    params = std::vector<std::string> (argv, argv + argc);
}

bool getCLParam(std::string const& param) {
    for (auto const& p : params) {
        if (p == param) {
            return true;
        }
    }
    return false;
}

std::string getCLParamByIndex(unsigned int i) {
    if (i < params.size()) {
        return params[i];
    }
    return "";
}
