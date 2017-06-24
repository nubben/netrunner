#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include "Node.h"
#include "TagNode.h"
#include <string>

class HTMLParser {
public:
    void parse(std::string html);
    void parseTag(std::string element, TagNode* tagNode);
};

#endif