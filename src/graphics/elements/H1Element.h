#ifndef H1ELEMENT_H
#define H1ELEMENT_H

#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class H1Element {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif