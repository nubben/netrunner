#include "StringUtils.h"

std::string getFilenameExtension(std::string const& fileName) {
    auto dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 != std::string::npos) {
        return fileName.substr(dotPos + 1);
    }
    return "";
}
