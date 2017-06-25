#ifndef H1ELEMENT_H
#define H1ELEMENT_H

#include "../../graphics/opengl/components/Component.h"
#include "../../graphics/opengl/components/TextComponent.h"
#include "../Node.h"

class H1Element {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif