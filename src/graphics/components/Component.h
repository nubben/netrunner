#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>
#include <vector>

class Component {
public:
    virtual ~Component();
    std::shared_ptr<Component> parent = nullptr;
    std::vector<std::shared_ptr<Component>> children;
    float x;
    float y;
    float height;
    float width;
    bool isInline = false;
};

#endif