#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include "Node.h"
#include "TagNode.h"
#include <string>

class HTMLParser {
public:
    void parse(const std::string &html) const;
    void parseTag(const std::string &element, TagNode* tagNode) const;
};

#endif