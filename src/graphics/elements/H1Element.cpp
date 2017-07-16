#include "H1Element.h"

std::unique_ptr<Component> H1Element::renderer(const Node &node, int y, int windowWidth, int windowHeight) {
    return std::make_unique<TextComponent>(static_cast<const TextNode&>(node).text, 0, y, 24, true, 0x000000FF, windowWidth, windowHeight);
}
