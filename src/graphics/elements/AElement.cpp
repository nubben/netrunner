#include "AElement.h"

std::unique_ptr<Component> AElement::renderer(const std::shared_ptr<Node> node, const int y, const int windowWidth, const int windowHeight) {
    TextNode *textNode = dynamic_cast<TextNode*>(node.get());
    if (textNode) {
        return std::make_unique<TextComponent>(textNode->text, 0, y, 12, false, 0x00FFFF, windowWidth, windowHeight);
    }
    return nullptr;
}
