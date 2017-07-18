#include "ComponentBuilder.h"
#include <iostream>

const std::unordered_map<std::string, std::shared_ptr<Element>> ComponentBuilder::elementMap {
    {"a", std::make_shared<AElement>()},
    {"blockquote", std::make_shared<BLOCKQUOTEElement>()},
    {"h1", std::make_shared<H1Element>()},
    {"h2", std::make_shared<H2Element>()},
    {"h3", std::make_shared<H3Element>()},
    {"li", std::make_shared<LIElement>()},
    {"p", std::make_shared<PElement>()},
    {"span", std::make_shared<SPANElement>()}
};

std::shared_ptr<Component> ComponentBuilder::build(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent, int windowWidth, int windowHeight) {
    std::shared_ptr<Component> component;
    std::string tag;

    if (node->nodeType == NodeType::TAG) {
        TagNode *tagNode = dynamic_cast<TagNode*>(node.get());
        if (tagNode) {
            tag = tagNode->tag;
        }
    }
    else if (node->nodeType == NodeType::TEXT) {
        TagNode *tagNode = dynamic_cast<TagNode*>(node->parent.get());
        if (tagNode) {
            tag = tagNode->tag;
        }
    }

    int y = parentComponent->y - parentComponent->height;

    std::unordered_map<std::string, std::shared_ptr<Element>>::const_iterator elementPair = elementMap.find(tag);
    if (elementPair != elementMap.end()) {
        std::shared_ptr<Element> element = elementPair->second;
        if (element->isInline) {
            y += parentComponent->height;
        }
        component = element->renderer(node, y, windowWidth, windowHeight);
    }

    if (!component) {
        component = std::make_unique<Component>();
        component->y = y;
    }

    return component;
}
