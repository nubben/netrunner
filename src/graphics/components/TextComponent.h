#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <GL/glew.h>
#include "../text/TextRasterizer.h"
#include "Component.h"
#include <array>
#include <memory>
#include <vector>

class TextComponent : public Component {
private:
    int fontSize;
    bool bold;
    unsigned int color;
    bool verticesDirty = false;
    const unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    std::unique_ptr<const Glyph[]> glyphs;
    std::vector<std::unique_ptr<float[]>> glyphVertices;
    std::vector<GLuint> vertexArrayObjects;
    std::vector<GLuint> vertexBufferObjects;
    GLuint elementBufferObject;
    std::vector<GLuint> textures;
public:
    std::string text;
    TextComponent(const std::string &rawText, const int rawX, const int rawY, const int size, const bool bolded, const unsigned int hexColor, const int windowWidth, const int windowHeight);
    ~TextComponent();
    void rasterize(const int rawX, const int rawY, const int windowWidth, const int windowHeight);
    void render();
    void resize(const int rawX, const int rawY, const int windowWidth, const int windowHeight);
    void pointToViewport(float &rawX, float &rawY, const int windowWidth, const int windowHeight) const;
    void sanitize(std::string &str);
};

#endif