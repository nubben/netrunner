#ifndef COMPONENT_H
#define COMPONENT_H

#include <functional>
#include <memory>
#include <vector>

class Component {
public:
    virtual ~Component();
    bool verticesDirty = false;
    std::shared_ptr<Component> parent = nullptr;
    std::vector<std::shared_ptr<Component>> children;
    float x;
    float y;
    float height;
    float width;
    bool isInline = false;
    std::function<void()> onClick = nullptr;
};

#endif