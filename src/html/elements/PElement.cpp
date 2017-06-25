#include "PElement.h"
#include "../TextNode.h"
#include <iostream>

std::unique_ptr<Component> PElement::render(const Node &node, int y, int windowWidth, int windowHeight) {
    TextNode *textNode = dynamic_cast<TextNode*>(node.children[0].get());
    return std::make_unique<TextComponent>(textNode->text, 0, y, 12, false, windowWidth, windowHeight);
}
