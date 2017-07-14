#include "ComponentBuilder.h"
#include <iostream>

const ElementRendererMap ComponentBuilder::tagRenderers {
};

const ElementRendererMap ComponentBuilder::textRenderers {
    {"a", &AElement::render},
    {"blockquote", &BLOCKQUOTEElement::render},
    {"h1", &H1Element::render},
    {"h2", &H2Element::render},
    {"h3", &H3Element::render},
    {"li", &LIElement::render},
    {"p", &PElement::render},
    {"span", &SPANElement::render}
};

std::unique_ptr<Component> ComponentBuilder::build(const std::shared_ptr<Node> node, int y, int windowWidth, int windowHeight) {
    std::unique_ptr<Component> component;
    if (node->nodeType == NodeType::TAG) {
        TagNode *tagNode = dynamic_cast<TagNode*>(node.get());
        if (tagNode) {
            ElementRendererMap::const_iterator tagRenderer = tagRenderers.find(tagNode->tag);
            if (tagRenderer != tagRenderers.end()) {
                component = tagRenderer->second(*tagNode, y, windowWidth, windowHeight);
            }
        }
    }
    else if (node->nodeType == NodeType::TEXT) {
        TagNode *tagNode = dynamic_cast<TagNode*>(node->parent.get());
        if (tagNode) {
            ElementRendererMap::const_iterator textRenderer = textRenderers.find(tagNode->tag);
            if (textRenderer != textRenderers.end()) {
                TextNode *textNode = dynamic_cast<TextNode*>(node.get());
                if (textNode) {
                    component = textRenderer->second(*textNode, y, windowWidth, windowHeight);
                }
            }
        }
    }
    return component;
}
