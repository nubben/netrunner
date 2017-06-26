#include "LIElement.h"
#include "../TextNode.h"
#include <iostream>

std::unique_ptr<Component> LIElement::render(const Node &node, int y, int windowWidth, int windowHeight) {
    if (!node.children.empty()) {
        TextNode *textNode = dynamic_cast<TextNode*>(node.children[0].get());
        if (textNode) {
            return std::make_unique<TextComponent>("• " + textNode->text, 0, y, 12, false, windowWidth, windowHeight);
        }
    }
    return nullptr;
}
