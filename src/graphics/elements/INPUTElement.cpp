#include "INPUTElement.h"

INPUTElement::INPUTElement() {
    isInline = true;
}

std::unique_ptr<Component> INPUTElement::renderer(const std::shared_ptr<Node> node, const int x, const int y, const int windowWidth, const int windowHeight) {
    // const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int windowWidth, const int windowHeight
    // what should our default size be?
    //std::cout << "INPUTElement::renderer - creating InputComponent at " << x << "x" << y << std::endl;
    std::unique_ptr<Component> component = std::make_unique<InputComponent>(x, y, 125.0f, 13.0f, windowWidth, windowHeight);
    return component;

    /*
    TextNode *textNode = dynamic_cast<TextNode*>(node.get());
    if (textNode) {
        if (node->parent->children.size() == 1) {
            std::unique_ptr<Component> component = std::make_unique<InputComponent>(textNode->text, x, y, 12, false, 0x000000FF, windowWidth, windowHeight);
            return component;
        }
    }
    */
    return nullptr;
}
