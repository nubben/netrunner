#include "StringUtils.h"

std::string getFilenameExtension(std::string const& fileName) {
    auto dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 != std::string::npos) {
        return fileName.substr(dotPos + 1);
    }
    return "";
}

const std::string getDocumentFromURL(const std::string &url) {
    int slashes = 0;
    for (unsigned int i = 0; i < url.length(); i++) {
        if (url[i] == '/') {
            slashes++;
            if (slashes == 3) {
                return url.substr(i, url.length() - i);
            }
        }
    }
    return "";
}

const std::string getHostFromURL(const std::string &url) {
    int slashes = 0;
    unsigned int start = 0;
    for (unsigned int i = 0; i < url.length(); i++) {
        if (url[i] == '/') {
            if (slashes == 2) {
                return url.substr(start, i - start);
            }
            slashes++;
            start = i + 1;
        }
    }
    return "";
}
