#include "SPANElement.h"

std::unique_ptr<Component> SPANElement::renderer(const std::shared_ptr<Node> node, const int y, const int windowWidth, const int windowHeight) {
    TextNode *textNode = dynamic_cast<TextNode*>(node.get());
    if (textNode) {
        if (node->parent->children.size() == 1) {
            std::unique_ptr<Component> component = std::make_unique<TextComponent>(textNode->text, 0, y, 12, false, 0x000000FF, windowWidth, windowHeight);
            return component;
        }
    }
    return nullptr;
}
