#include "AElement.h"
#include "../../html/TagNode.h"
#include <iostream>

AElement::AElement() {
    isInline = true;
}

std::unique_ptr<Component> AElement::renderer(const std::shared_ptr<Node> node, const int x, const int y, const int windowWidth, const int windowHeight) {
    TextNode *textNode = dynamic_cast<TextNode*>(node.get());
    if (textNode) {
        std::unique_ptr<Component> component = std::make_unique<TextComponent>(textNode->text, x, y, 12, false, 0x00FFFF, windowWidth, windowHeight);
        TagNode *tagNode = dynamic_cast<TagNode*>(textNode->parent.get());
        if (tagNode) {
            std::map<std::string, std::string>::const_iterator hrefPair = tagNode->properties.find("href");
            if (hrefPair != tagNode->properties.end()) {
                component->onClick = [hrefPair]() {
                    std::cout << "Direct to: " << hrefPair->second << std::endl;
                };
            }
        }
        return component;
    }
    return nullptr;
}
