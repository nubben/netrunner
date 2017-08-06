#ifndef INPUTELEMENT_H
#define INPUTELEMENT_H

#include "Element.h"
#include "../components/Component.h"
#include "../components/InputComponent.h"
#include "../../html/TextNode.h"

class INPUTElement : public Element {
public:
    INPUTElement();
    virtual std::unique_ptr<Component> renderer(const std::shared_ptr<Node> node, const int x, const int y, const int windowWidth, const int windowHeight);
};

#endif
