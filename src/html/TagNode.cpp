#include "TagNode.h"

const Element TagNode::elements[] = {
};

TagNode::TagNode() : Node(NodeType::TAG) {
}

std::unique_ptr<Component> TagNode::render(const Node &node, int y, int windowWidth, int windowHeight) {
    for (Element element : elements) {
        if (element.tag == tag) {
            return element.render(node, y, windowWidth, windowHeight);
        }
    }
    return nullptr;
}
