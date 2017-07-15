#ifndef BLOCKQUOTEELEMENT_H
#define BLOCKQUOTEELEMENT_H

#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class BLOCKQUOTEElement {
public:
    static std::unique_ptr<Component> render(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif