#ifndef TEXTNODE_H
#define TEXTNODE_H

#include "Node.h"
#include <map>

class TextNode : public Node {
private:
    static const Element elements[];
public:
    TextNode();
    std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
    std::string text;
};

#endif
