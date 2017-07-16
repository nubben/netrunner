#ifndef ELEMENT_H
#define ELEMENT_H

#include <functional>
#include <memory>
#include "../components/Component.h"
#include "../../html/Node.h"

class Element {
public:
    bool isInline = false;
    virtual ~Element();
    virtual std::unique_ptr<Component> renderer(const Node &node, int y, int windowWidth, int windowHeight);
};

#endif
