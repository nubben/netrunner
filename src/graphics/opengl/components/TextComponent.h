#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <GL/glew.h>
#include "../../text/TextRasterizer.h"
#include "Component.h"
#include <array>
#include <memory>
#include <vector>

class TextComponent : public Component {
private:
    std::string text;
    float x;
    float y;
    int fontSize;
    bool bold;
    bool verticesDirty = false;
    const unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned char data[1024][1024][4];
    std::unique_ptr<const Glyph[]> glyphs;
    std::vector<std::unique_ptr<float[]>> glyphVertices;
    std::vector<GLuint> vertexArrayObjects;
    std::vector<GLuint> vertexBufferObjects;
    std::vector<GLuint> elementBufferObjects;
    std::vector<GLuint> textures;
public:
    TextComponent(const std::string &text, const int x, const int y, const int fontSize, const bool bold,const int windowWidth, const int windowHeight);
    ~TextComponent();
    void rasterize(const std::string &text, const int x, const int y, const int fontSize, const bool bold, const int windowWidth, const int windowHeight);
    void render();
    void resize(const int x, const int y, const int windowWidth, const int windowHeight);
    void pointToViewport(float &x, float &y, const int windowWidth, const int windowHeight) const;
};

#endif