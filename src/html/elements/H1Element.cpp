#include "H1Element.h"
#include "../TextNode.h"
#include <iostream>

std::unique_ptr<Component> H1Element::render(const Node &node, int y, int windowWidth, int windowHeight) {
    if (!node.children.empty()) {
        TextNode *textNode = dynamic_cast<TextNode*>(node.children[0].get());
        if (textNode) {
            return std::make_unique<TextComponent>(textNode->text, 0, y, 24, true, windowWidth, windowHeight);
        }
    }
    return nullptr;
}
