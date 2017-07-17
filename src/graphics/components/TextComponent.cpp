#include "TextComponent.h"
#include <iostream>
#include <ctime>

TextComponent::TextComponent(const std::string &rawText, const int rawX, const int rawY, const int size, const bool bolded, const unsigned int hexColor, const int windowWidth, const int windowHeight) {
    text = rawText;
    x = rawX;
    y = rawY;
    fontSize = size;
    bold = bolded;
    color = hexColor;

    sanitize(text);

    rasterize(rawX, rawY, windowWidth, windowHeight);

    glGenBuffers(1, &elementBufferObject);

    for (unsigned int i = 0; i < glyphVertices.size(); i++) {
        const Glyph &glyph = glyphs[i];
        const std::unique_ptr<float[]> &glyphVertice = glyphVertices[i];
        vertexArrayObjects.push_back(0);
        vertexBufferObjects.push_back(0);
        glGenVertexArrays(1, &vertexArrayObjects.back());
        glGenBuffers(1, &vertexBufferObjects.back());

        glBindVertexArray(vertexArrayObjects.back());

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects.back());
        glBufferData(GL_ARRAY_BUFFER, ((3 + 4 + 2) * 4) * sizeof(float), glyphVertice.get(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), reinterpret_cast<void*>(7 * sizeof(float)));
        glEnableVertexAttribArray(2);

        textures.push_back(0);
        glGenTextures(1, &textures.back());
        glBindTexture(GL_TEXTURE_2D, textures.back());

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glyph.textureWidth, glyph.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, glyph.textureData.get());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

TextComponent::~TextComponent() {
    glDeleteBuffers(1, &elementBufferObject);
    for (unsigned int i = 0; i < vertexArrayObjects.size(); i++) {
        glDeleteVertexArrays(1, &vertexArrayObjects[i]);
        glDeleteBuffers(1, &vertexBufferObjects[i]);
        glDeleteTextures(1, &textures[i]);
    }
}

void TextComponent::rasterize(const int rawX, const int rawY, const int windowWidth, const int windowHeight) {
    const std::unique_ptr<TextRasterizer> textRasterizer = std::make_unique<TextRasterizer>("DejaVuSerif.ttf", fontSize, 72, bold);
    unsigned int glyphCount;
    glyphs = textRasterizer->rasterize(text, rawX, rawY, windowWidth, windowHeight, height, glyphCount);
    if (glyphs == nullptr) {
        return;
    }

    glyphVertices.clear();
    for (unsigned int i = 0; i < glyphCount; i++) {
        const Glyph &glyph = glyphs[i];

        float vx0 = glyph.x0;
        float vy0 = glyph.y0;
        float vx1 = glyph.x1;
        float vy1 = glyph.y1;
        pointToViewport(vx0, vy0, windowWidth, windowHeight);
        pointToViewport(vx1, vy1, windowWidth, windowHeight);

        std::unique_ptr<float[]> vertices = std::make_unique<float[]>(36);
        vertices[(0 * (3 + 4 + 2)) + 0] = vx0;
        vertices[(0 * (3 + 4 + 2)) + 1] = vy0;
        vertices[(0 * (3 + 4 + 2)) + 2] = 0.0f;
        vertices[(0 * (3 + 4 + 2)) + 3] = (static_cast<float>((color >> 24) & 0xFF)) / 255;
        vertices[(0 * (3 + 4 + 2)) + 4] = (static_cast<float>((color >> 16) & 0xFF)) / 255;
        vertices[(0 * (3 + 4 + 2)) + 5] = (static_cast<float>((color >>  8) & 0xFF)) / 255;
        vertices[(0 * (3 + 4 + 2)) + 6] = (static_cast<float>((color >>  0) & 0xFF)) / 255;
        vertices[(0 * (3 + 4 + 2)) + 7] = glyph.s0;
        vertices[(0 * (3 + 4 + 2)) + 8] = glyph.t0;
        vertices[(1 * (3 + 4 + 2)) + 0] = vx0;
        vertices[(1 * (3 + 4 + 2)) + 1] = vy1;
        vertices[(1 * (3 + 4 + 2)) + 2] = 0.0f;
        vertices[(1 * (3 + 4 + 2)) + 3] = (static_cast<float>((color >> 24) & 0xFF)) / 255;
        vertices[(1 * (3 + 4 + 2)) + 4] = (static_cast<float>((color >> 16) & 0xFF)) / 255;
        vertices[(1 * (3 + 4 + 2)) + 5] = (static_cast<float>((color >>  8) & 0xFF)) / 255;
        vertices[(1 * (3 + 4 + 2)) + 6] = (static_cast<float>((color >>  0) & 0xFF)) / 255;
        vertices[(1 * (3 + 4 + 2)) + 7] = glyph.s0;
        vertices[(1 * (3 + 4 + 2)) + 8] = glyph.t1;
        vertices[(2 * (3 + 4 + 2)) + 0] = vx1;
        vertices[(2 * (3 + 4 + 2)) + 1] = vy1;
        vertices[(2 * (3 + 4 + 2)) + 2] = 0.0f;
        vertices[(2 * (3 + 4 + 2)) + 3] = (static_cast<float>((color >> 24) & 0xFF)) / 255;
        vertices[(2 * (3 + 4 + 2)) + 4] = (static_cast<float>((color >> 16) & 0xFF)) / 255;
        vertices[(2 * (3 + 4 + 2)) + 5] = (static_cast<float>((color >>  8) & 0xFF)) / 255;
        vertices[(2 * (3 + 4 + 2)) + 6] = (static_cast<float>((color >>  0) & 0xFF)) / 255;
        vertices[(2 * (3 + 4 + 2)) + 7] = glyph.s1;
        vertices[(2 * (3 + 4 + 2)) + 8] = glyph.t1;
        vertices[(3 * (3 + 4 + 2)) + 0] = vx1;
        vertices[(3 * (3 + 4 + 2)) + 1] = vy0;
        vertices[(3 * (3 + 4 + 2)) + 2] = 0.0f;
        vertices[(3 * (3 + 4 + 2)) + 3] = (static_cast<float>((color >> 24) & 0xFF)) / 255;
        vertices[(3 * (3 + 4 + 2)) + 4] = (static_cast<float>((color >> 16) & 0xFF)) / 255;
        vertices[(3 * (3 + 4 + 2)) + 5] = (static_cast<float>((color >>  8) & 0xFF)) / 255;
        vertices[(3 * (3 + 4 + 2)) + 6] = (static_cast<float>((color >>  0) & 0xFF)) / 255;
        vertices[(3 * (3 + 4 + 2)) + 7] = glyph.s1;
        vertices[(3 * (3 + 4 + 2)) + 8] = glyph.t0;

        glyphVertices.push_back(std::move(vertices));
    }
}

void TextComponent::render() {
    if (verticesDirty) {
        for (unsigned int i = 0; i < vertexBufferObjects.size(); i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[i]);
            glBufferData(GL_ARRAY_BUFFER, ((3 + 4 + 2) * 4) * sizeof(float), glyphVertices[i].get(), GL_STATIC_DRAW);
        }
        verticesDirty = false;
    }
    for (unsigned int i = vertexArrayObjects.size(); i > 0; i--) {
        glBindVertexArray(vertexArrayObjects[i - 1]);
        glBindTexture(GL_TEXTURE_2D, textures[i - 1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}

void TextComponent::resize(const int rawX, const int rawY, const int windowWidth, const int windowHeight) {
    y = rawY;
    rasterize(rawX, rawY, windowWidth, windowHeight);
    verticesDirty = true;
}

void TextComponent::pointToViewport(float &rawX, float &rawY, const int windowWidth, const int windowHeight) const {
    rawX = ((rawX / windowWidth) * 2) - 1;
    rawY = ((rawY / windowHeight) * 2) - 1;
}

void TextComponent::sanitize(std::string &str) {
    size_t found = 0;
    while ((found = str.find("&", found)) != std::string::npos) {
        if (str.substr(found, 4) == "&gt;") {
            str.replace(found, 4, ">");
        }
        found++;
    }
    found = 0;
    while ((found = str.find("\n", found)) != std::string::npos) {
        str.replace(found, 1, "");
        found++;
    }
}
