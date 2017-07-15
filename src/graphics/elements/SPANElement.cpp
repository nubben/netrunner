#include "SPANElement.h"

std::unique_ptr<Component> SPANElement::render(const Node &node, int y, int windowWidth, int windowHeight) {
    if (node.parent->children.size() == 1) {
        return std::make_unique<TextComponent>(static_cast<const TextNode&>(node).text, 0, y, 12, false, 0x000000FF, windowWidth, windowHeight);
    }
    return nullptr;
}
