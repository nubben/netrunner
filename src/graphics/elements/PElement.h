#ifndef PELEMENT_H
#define PELEMENT_H

#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class PElement {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif