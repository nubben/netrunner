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
    const unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned char data[1024][1024][4];
    GLuint vertexArrayObject = 0;
    GLuint vertexBufferObject = 0;
    GLuint elementBufferObject = 0;
    GLuint texture = 0;
public:
    BoxComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int passedWindowWidth, const int passedWindowHeight);
    ~BoxComponent();
    void render();
    void resize();
    void pointToViewport(float &rawX, float &rawY, const int passedWindowWidth, const int passedWindowHeight) const ;
    void distanceToViewport(float &rawX, float &rawY, const int passedWindowWidth, const int passedWindowHeight) const ;
};

#endif
