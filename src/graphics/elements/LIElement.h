#ifndef LIELEMENT_H
#define LIELEMENT_H

#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class LIElement {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif