#include "TextNode.h"
#include "TagNode.h"
#include "elements/AElement.h"
#include "elements/BLOCKQUOTEElement.h"
#include "elements/H1Element.h"
#include "elements/H2Element.h"
#include "elements/H3Element.h"
#include "elements/LIElement.h"
#include "elements/PElement.h"
#include "elements/SPANElement.h"

const Element TextNode::elements[] = {
    {"a", &AElement::render},
    {"blockquote", &BLOCKQUOTEElement::render},
    {"h1", &H1Element::render},
    {"h2", &H2Element::render},
    {"h3", &H3Element::render},
    {"li", &LIElement::render},
    {"p", &PElement::render},
    {"span", &SPANElement::render}
};

TextNode::TextNode() : Node(NodeType::TEXT) {
}
#include <iostream>
std::unique_ptr<Component> TextNode::render(const Node &node, int y, int windowWidth, int windowHeight) {
    TagNode *tagNode = dynamic_cast<TagNode*>(node.parent.get());
    if (tagNode) {
        for (Element element : elements) {
            if (element.tag == tagNode->tag) {
                return element.render(node, y, windowWidth, windowHeight);
            }
        }
    }
    return nullptr;
}
