#ifndef COMPONENT_H
#define COMPONENT_H

class Component {
public:
    virtual ~Component();
    bool verticesDirty = false;
    float x;
    float y;
    float height;
    float width;
};

#endif