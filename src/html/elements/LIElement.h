#ifndef LIELEMENT_H
#define LIELEMENT_H

#include "../../graphics/opengl/components/Component.h"
#include "../../graphics/opengl/components/TextComponent.h"
#include "../TextNode.h"

class LIElement {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif