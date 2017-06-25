#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <GL/glew.h>
#include "Component.h"
#include <array>
#include <memory>
#include <vector>

class TextComponent : public Component {
private:
    float x;
    float y;
    float width;
    bool verticesDirty = false;
    const unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned char data[1024][1024][4];
    std::vector<std::unique_ptr<float[]>> glyphVertices;
    std::vector<GLuint> vertexArrayObjects;
    std::vector<GLuint> vertexBufferObjects;
    std::vector<GLuint> elementBufferObjects;
    std::vector<GLuint> textures;
public:
    TextComponent(const std::string &text, const int x, const int y, const int fontSize, const bool bold,const int windowWidth, const int windowHeight);
    ~TextComponent();
    void render();
    void resize(const float sx, const float sy);
    void pointToViewport(float &x, float &y, const int windowWidth, const int windowHeight) const;
};

#endif