#ifndef TEXTNODE_H
#define TEXTNODE_H

#include "Node.h"
#include <map>

class TextNode : public Node {
public:
    TextNode();
    std::string text;
};

#endif
