#include "TextComponent.h"
#include "../../text/TextRasterizer.h"
#include <memory>

TextComponent::TextComponent(const std::string &text, const int x, const int y, const int fontSize, const bool bold, const int windowWidth, const int windowHeight) {
    this->x = x;
    this->y = y;

    const std::unique_ptr<TextRasterizer> textRasterizer = std::make_unique<TextRasterizer>("DejaVuSerif.ttf", fontSize, 72, bold);
    unsigned int glyphCount;
    std::unique_ptr<const Glyph[]> glyphs = textRasterizer->rasterize(text, x, y, height, glyphCount);
    if (glyphs ==  nullptr) {
        return;
    }
    for (int i = 0; i < glyphCount; i++) {
        const Glyph &glyph = glyphs[i];

        float vx0 = glyph.x0;
        float vy0 = glyph.y0;
        float vx1 = glyph.x1;
        float vy1 = glyph.y1;
        pointToViewport(vx0, vy0, windowWidth, windowHeight);
        pointToViewport(vx1, vy1, windowWidth, windowHeight);

        std::unique_ptr<float[]> vertices = std::make_unique<float[]>(20);
        vertices[(0 * 5) + 0] = vx0;
        vertices[(0 * 5) + 1] = vy0;
        vertices[(0 * 5) + 2] = 0.0f;
        vertices[(0 * 5) + 3] = glyph.s0;
        vertices[(0 * 5) + 4] = glyph.t0;
        vertices[(1 * 5) + 0] = vx0;
        vertices[(1 * 5) + 1] = vy1;
        vertices[(1 * 5) + 2] = 0.0f;
        vertices[(1 * 5) + 3] = glyph.s0;
        vertices[(1 * 5) + 4] = glyph.t1;
        vertices[(2 * 5) + 0] = vx1;
        vertices[(2 * 5) + 1] = vy1;
        vertices[(2 * 5) + 2] = 0.0f;
        vertices[(2 * 5) + 3] = glyph.s1;
        vertices[(2 * 5) + 4] = glyph.t1;
        vertices[(3 * 5) + 0] = vx1;
        vertices[(3 * 5) + 1] = vy0;
        vertices[(3 * 5) + 2] = 0.0f;
        vertices[(3 * 5) + 3] = glyph.s1;
        vertices[(3 * 5) + 4] = glyph.t0;
        glyphVertices.push_back(std::move(vertices));

        vertexArrayObjects.push_back(0);
        vertexBufferObjects.push_back(0);
        elementBufferObjects.push_back(0);
        glGenVertexArrays(1, &vertexArrayObjects.back());
        glGenBuffers(1, &vertexBufferObjects.back());
        glGenBuffers(1, &elementBufferObjects.back());

        glBindVertexArray(vertexArrayObjects.back());

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects.back());
        glBufferData(GL_ARRAY_BUFFER, ((3 + 2) * 4) * sizeof(float), glyphVertices.back().get(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects.back());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (0 * sizeof(float)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        textures.push_back(0);
        glGenTextures(1, &textures.back());
        glBindTexture(GL_TEXTURE_2D, textures.back());

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glyph.textureWidth, glyph.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, glyph.textureData.get());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

TextComponent::~TextComponent() {
    for (int i = 0; i < vertexArrayObjects.size(); i++) {
        glDeleteVertexArrays(1, &vertexArrayObjects[i]);
        glDeleteBuffers(1, &vertexBufferObjects[i]);
        glDeleteBuffers(1, &elementBufferObjects[i]);
        glDeleteTextures(1, &textures[i]);
    }
}

void TextComponent::render() {
    if (verticesDirty) {
        for (int i = 0; i < vertexArrayObjects.size(); i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[i]);
            glBufferData(GL_ARRAY_BUFFER, ((3 + 2) * 4) * sizeof(float), glyphVertices[i].get(), GL_STATIC_DRAW);
        }
        verticesDirty = false;
    }
    for (int i = vertexArrayObjects.size() - 1; i >= 0; i--) {
        glBindVertexArray(vertexArrayObjects[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void TextComponent::resize(const float sx, const float sy) {
    for (int i = 0; i < glyphVertices.size(); i++) {
        glyphVertices[i][(0 * 5) + 0] = ((glyphVertices[i][(0 * 5) + 0] + 1) / sx) - 1;
        glyphVertices[i][(0 * 5) + 1] = ((glyphVertices[i][(0 * 5) + 1] + 1) / sy) - 1;
        glyphVertices[i][(1 * 5) + 0] = ((glyphVertices[i][(1 * 5) + 0] + 1) / sx) - 1;
        glyphVertices[i][(1 * 5) + 1] = ((glyphVertices[i][(1 * 5) + 1] + 1) / sy) - 1;
        glyphVertices[i][(2 * 5) + 0] = ((glyphVertices[i][(2 * 5) + 0] + 1) / sx) - 1;
        glyphVertices[i][(2 * 5) + 1] = ((glyphVertices[i][(2 * 5) + 1] + 1) / sy) - 1;
        glyphVertices[i][(3 * 5) + 0] = ((glyphVertices[i][(3 * 5) + 0] + 1) / sx) - 1;
        glyphVertices[i][(3 * 5) + 1] = ((glyphVertices[i][(3 * 5) + 1] + 1) / sy) - 1;
    }
    verticesDirty = true;
}

void TextComponent::pointToViewport(float &x, float &y, const int windowWidth, const int windowHeight) const {
    x = ((x / windowWidth) * 2) - 1;
    y = ((y / windowHeight) * 2) - 1;
}
