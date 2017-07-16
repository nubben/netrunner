#include "ComponentBuilder.h"
#include <iostream>

//const ElementRendererMap ComponentBuilder::tagRenderers {
//};
//
//const ElementRendererMap ComponentBuilder::textRenderers {
//};

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

std::unique_ptr<Component> ComponentBuilder::build(const std::shared_ptr<Node> node, const std::unique_ptr<Component> &parentComponent, int windowWidth, int windowHeight) {
    std::unique_ptr<Component> component;
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

    std::unordered_map<std::string, std::shared_ptr<Element>>::const_iterator elementPair = elementMap.find(tag);
    if (elementPair != elementMap.end()) {
        std::shared_ptr<Element> element = elementPair->second;
        int y = parentComponent != nullptr ? (parentComponent->y + (!element->isInline ? 0 : parentComponent->y)) : 0;
        y += 500;
        std::cout << tag << std::endl;
        std::cout << y << std::endl;
        component = element->renderer(*node.get(), y, windowWidth, windowHeight);
    }

    return component;
}
