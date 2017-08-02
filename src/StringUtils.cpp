#include "StringUtils.h"
#include <iostream>

#include <iterator>
#include <algorithm>

/**
 * get an extension from a filename
 * @param fileName a filename string
 * @return '' or a string with the found extension
 */
std::string getFilenameExtension(std::string const& fileName) {
    auto dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 != std::string::npos) {
        //std::cout << "StringUtils::getFilenameExtension - " << fileName.substr(dotPos + 1) << std::endl;
        return fileName.substr(dotPos + 1);
    }
    return "";
}

/**
 * get an document from a url
 * @param url url string
 * @return '' or a string with the found document
 */
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

/**
 * get host from a url
 * @param url url string
 * @return '' or a string with the found host
 */
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

/**
 * convert string to lowercase
 * @param str string
 * @return lowercased version of str
 */
const std::string toLowercase(const std::string &str) {
    std::string returnString = "";
    std::transform(str.begin(),
                   str.end(),
                   back_inserter(returnString),
                   tolower);
    return returnString;
}
