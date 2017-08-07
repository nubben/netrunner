#ifndef BOXCOMPONENT_H
#define BOXCOMPONENT_H

#include <GL/glew.h>
#include "Component.h"

class BoxComponent : public Component {
protected:
    float vertices[20] = {
        0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f
    };
    unsigned char data[1][1][4];
    GLuint vertexArrayObject = 0;
    GLuint vertexBufferObject = 0;
    GLuint elementBufferObject = 0;
    GLuint texture = 0;
    int initialX;
    int initialY;
    int initialWidth;
    int initialHeight;
    int initialWindowWidth;
    int initialWindowHeight;
public:
    BoxComponent();
    BoxComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int passedWindowWidth, const int passedWindowHeight);
    ~BoxComponent();
    virtual void render();
    //void resize(const int passedWindowWidth, const int passedWindowHeight);
};

#endif
