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

std::string getRelativeCLParam(std::string const& param, int i) {
    for (unsigned int index = 0; index < params.size(); ++index) {
        if (params[index] == param) {
            int si = static_cast<int>(index);
            unsigned int finalPos = static_cast<unsigned int>(si + i);
            if (finalPos < params.size()) {
                return params[finalPos];
            }
            return "";
        }
    }
    return "";
}
