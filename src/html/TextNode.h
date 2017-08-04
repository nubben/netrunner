#ifndef TEXTNODE_H
#define TEXTNODE_H

#include "Node.h"

class TextNode : public Node {
public:
    TextNode();
    std::string text;

    std::vector<std::string> getSourceList() override { return {}; };
};

#endif
