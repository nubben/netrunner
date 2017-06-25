#include "H3Element.h"
#include "../TextNode.h"
#include <iostream>

std::unique_ptr<Component> H3Element::render(const Node &node, int y, int windowWidth, int windowHeight) {
    TextNode *textNode = dynamic_cast<TextNode*>(node.children[0].get());
    return std::make_unique<TextComponent>(textNode->text, 0, y, 14, true, windowWidth, windowHeight); // Should be 14.04pt
}
