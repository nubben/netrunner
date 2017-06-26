#ifndef SPANELEMENT_H
#define SPANELEMENT_H

#include "../../graphics/opengl/components/Component.h"
#include "../../graphics/opengl/components/TextComponent.h"
#include "../TextNode.h"

class SPANElement {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif