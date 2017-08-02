#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include "Node.h"
#include "TagNode.h"
#include <string>

void printNode(const std::shared_ptr<Node> node, const int indent);

class HTMLParser {
public:
    std::shared_ptr<Node> parse(const std::string &html) const;
    void parseTag(const std::string &element, TagNode &tagNode) const;
};

#endif
