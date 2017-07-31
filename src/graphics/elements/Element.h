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
    // TODO: remove position
    // add parent (node or component? well component because that's how the create component is going to calculate position
    // well right now ComponentBuilder calls renderer and it has parent/window size
    // component constructors don't really need parent/window size
    // so we could remove window size too
    virtual std::unique_ptr<Component> renderer(const std::shared_ptr<Node> node, const int x, const int y, const int windowWidth, const int windowHeight);
};

#endif
