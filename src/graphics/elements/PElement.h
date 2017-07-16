#ifndef PELEMENT_H
#define PELEMENT_H

#include "Element.h"
#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class PElement : public Element {
public:
    virtual std::unique_ptr<Component> renderer(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif
