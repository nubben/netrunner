#ifndef H2ELEMENT_H
#define H2ELEMENT_H

#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class H2Element {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif