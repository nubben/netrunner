#ifndef H3ELEMENT_H
#define H3ELEMENT_H

#include "Element.h"
#include "../components/Component.h"
#include "../components/TextComponent.h"
#include "../../html/TextNode.h"

class H3Element : public Element {
public:
    virtual std::unique_ptr<Component> renderer(const std::shared_ptr<Node> node, const int y, const int windowWidth, const int windowHeight);
};

#endif
