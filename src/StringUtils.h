#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
const std::string toLowercase(const std::string &str);

// should be moved to URL
const std::string getFilenameExtension(std::string const& fileName);
const std::string getDocumentFromURL(const std::string &url);
const std::string getHostFromURL(const std::string &url);
const std::string getSchemeFromURL(const std::string &url);

#endif
