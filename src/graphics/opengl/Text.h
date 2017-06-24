#ifndef TEXT_H
#define TEXT_H

#include <GL/glew.h>
#include <array>
#include <vector>

class Text {
private:
    float x;
    float y;
    float width;
    float height;
    bool verticesDirty = false;
    unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned char data[1024][1024][4];
    std::vector<float*> glyphVertices;
    std::vector<GLuint> vertexArrayObjects;
    std::vector<GLuint> vertexBufferObjects;
    std::vector<GLuint> elementBufferObjects;
    std::vector<GLuint> textures;
public:
    Text(std::string text, int x, int y, int windowWidth, int windowHeight);
    ~Text();
    void render();
    void resize(float sx, float sy);
    void pointToViewport(float &x, float &y, int windowWidth, int windowHeight);
};

#endif