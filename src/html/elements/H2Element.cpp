#include "H2Element.h"

std::unique_ptr<Component> H2Element::render(const Node &node, int y, int windowWidth, int windowHeight) {
    return std::make_unique<TextComponent>(static_cast<const TextNode&>(node).text, 0, y, 18, true, windowWidth, windowHeight);
}
