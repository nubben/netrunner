#include "TagNode.h"
#include "elements/H1Element.h"
#include "elements/H2Element.h"
#include "elements/H3Element.h"
#include "elements/LIElement.h"
#include "elements/PElement.h"
#include "elements/SPANElement.h"

const Element TagNode::elements[] = {
    {"h1", &H1Element::render},
    {"h2", &H2Element::render},
    {"h3", &H3Element::render},
    {"li", &LIElement::render},
    {"p", &PElement::render},
    {"span", &SPANElement::render}
};

TagNode::TagNode() : Node(NodeType::TAG) {

}

std::unique_ptr<Component> TagNode::render(const Node &node, int y, int windowWidth, int windowHeight) {
    for (Element element : elements) {
        if (tag == element.tag) {
            return element.render(node, y, windowWidth, windowHeight);
        }
    }
    return nullptr;
}
