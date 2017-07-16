#ifndef AELEMENT_H
#define AELEMENT_H

#include "Element.h"
#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class AElement : public Element {
public:
    bool isInline = true;
    virtual std::unique_ptr<Component> renderer(const std::shared_ptr<Node> node, const int y, const int windowWidth, const int windowHeight);
};

#endif
