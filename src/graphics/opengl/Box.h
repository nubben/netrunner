#ifndef BOX_H
#define BOX_H

#include <GL/glew.h>

class Box {
private:
    float x;
    float y;
    float width;
    float height;
    float vertices[20] = {
        0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f
    };
    bool verticesDirty = false;
    unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned char data[1024][1024][4];
    GLuint vertexArrayObject = 0;
    GLuint vertexBufferObject = 0;
    GLuint elementBufferObject = 0;
    GLuint texture = 0;
public:
    Box(float x, float y, float width, float height, int windowWidth, int windowHeight);
    ~Box();
    void render();
    void resize(int width, int height);
    void pointToViewport(float &x, float &y, int windowWidth, int windowHeight);
    void distanceToViewport(float &x, float &y, int windowWidth, int windowHeight);
};

#endif