#include "LIElement.h"

std::unique_ptr<Component> LIElement::render(const Node &node, int y, int windowWidth, int windowHeight) {
    return std::make_unique<TextComponent>("• " + static_cast<const TextNode&>(node).text, 0, y, 12, false, windowWidth, windowHeight);
}
