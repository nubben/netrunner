#ifndef AELEMENT_H
#define AELEMENT_H

#include "Element.h"
#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

void navTo(std::string url); // forward declaration

class AElement : public Element {
public:
    AElement();
    virtual std::unique_ptr<Component> renderer(const std::shared_ptr<Node> node, const int x, const int y, const int windowWidth, const int windowHeight);
};

#endif
