#include "AElement.h"

AElement::AElement() {
    isInline = true;
}

std::unique_ptr<Component> AElement::renderer(const std::shared_ptr<Node> node, const int x, const int y, const int windowWidth, const int windowHeight) {
    TextNode *textNode = dynamic_cast<TextNode*>(node.get());
    if (textNode) {
        return std::make_unique<TextComponent>(textNode->text, x, y, 12, false, 0x00FFFF, windowWidth, windowHeight);
    }
    return nullptr;
}
