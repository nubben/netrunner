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
    float x=0.0f;
    float y=0.0f;
    float height=0.0f;
    float width=0.0f;
    bool isInline = false;
    std::function<void()> onClick = nullptr;
};

#endif
