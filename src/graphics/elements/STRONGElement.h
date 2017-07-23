#ifndef STRONGELEMENT_H
#define STRONGELEMENT_H

#include "Element.h"
#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class STRONGElement : public Element {
public:
    STRONGElement();
    virtual std::unique_ptr<Component> renderer(const std::shared_ptr<Node> node, const int x, const int y, const int windowWidth, const int windowHeight);
};

#endif
