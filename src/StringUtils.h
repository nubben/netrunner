#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>

std::string getFilenameExtension(std::string const& fileName);
const std::string getDocumentFromURL(const std::string &url);
const std::string getHostFromURL(const std::string &url);
std::string stringToLower(const std::string s);

#endif
