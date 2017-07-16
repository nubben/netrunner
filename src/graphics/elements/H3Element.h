#ifndef H3ELEMENT_H
#define H3ELEMENT_H

#include "Element.h"
#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class H3Element : public Element {
public:
    virtual std::unique_ptr<Component> renderer(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif
