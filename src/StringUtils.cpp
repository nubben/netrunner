#include "StringUtils.h"

#include <algorithm>
#include <iostream>

#include <iterator>
#include <algorithm>

/**
 * get an extension from a filename
 * @param fileName a filename string
 * @return '' or a string with the found extension
 */
const std::string getFilenameExtension(std::string const& fileName) {
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
    auto colonDoubleSlash = url.find("://");
    if (colonDoubleSlash != std::string::npos) {
        auto startPos = colonDoubleSlash + 3;
        auto thirdSlash = url.find("/", startPos);
        if (thirdSlash == std::string::npos) {
            return url.substr(startPos);
        }
        return url.substr(startPos, thirdSlash - startPos);
    }
    return "";
}

/**
 * get protocol from a url
 * @param url url string
 * @return '' or a string with the found protocol
 */
const std::string getProtocolFromURL(const std::string &url) {
    auto colonDoubleSlash = url.find("://");
    if (colonDoubleSlash != std::string::npos) {
        return url.substr(0, colonDoubleSlash);
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
/*
    std::string stringToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c){ return std::tolower(c); });
    return s;
*/
}
