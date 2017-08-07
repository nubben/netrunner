#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <GL/glew.h>
#include "../text/TextRasterizerCache.h"
#include "Component.h"
#include <array>
#include <memory>
#include <vector>
#include <iostream>

class TextComponent : public Component {
private:
    unsigned int fontSize;
    bool bold;
    unsigned int color;
    std::unique_ptr<Glyph[]> glyphs;
    std::vector<std::unique_ptr<float[]>> glyphVertices;
    std::vector<GLuint> vertexArrayObjects;
    std::vector<GLuint> vertexBufferObjects;
    GLuint elementBufferObject;
    std::vector<GLuint> textures;

public:
    std::string text;
    TextComponent(const std::string &rawText, const int rawX, const int rawY, const unsigned int size, const bool bolded, const unsigned int hexColor, const int passedWindowWidth, const int passedWindowHeight);
    ~TextComponent();
    void rasterize(const int rawX, const int rawY);
    void render();
    void resize(const int passedWindowWidth, const int passedWindowHeight); // compatible adapter
    void resize(const int passedWindowWidth, const int passedWindowHeight, const int passedAvailableWidth); // more detailed control
    void sanitize(std::string &str);
    
    // input needed stuff
    int rasterStartX = 0; // start reading text source at and place at destination 0
    int rasterStartY = 0;
    bool noWrap = false; // different than overflow but related
    int availableWidth = 0;
    
    GLsizei textureWidth;
    GLsizei textureHeight;
    std::unique_ptr<unsigned char[]> textureData;

    // backgroundColor
    rgba backgroundColor;
};

#endif
