#ifndef BOXCOMPONENT_H
#define BOXCOMPONENT_H

#include <GL/glew.h>
#include "Component.h"

class BoxComponent : public Component {
private:
    float x;
    float y;
    float width;
    float vertices[20] = {
        0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f
    };
    bool verticesDirty = false;
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
    BoxComponent(const float x, const float y, const float width, const float height, const int windowWidth, const int windowHeight);
    ~BoxComponent();
    void render();
    void resize(const int width, const int height);
    void pointToViewport(float &x, float &y, const int windowWidth, const int windowHeight) const ;
    void distanceToViewport(float &x, float &y, const int windowWidth, const int windowHeight) const ;
};

#endif