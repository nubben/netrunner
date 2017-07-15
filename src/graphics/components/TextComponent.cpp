#include "TextComponent.h"
#include <iostream>
#include <ctime>

extern TextRasterizerCache *rasterizerCache;

TextComponent::TextComponent(const std::string &rawText, const int rawX, const int rawY, const int size, const bool bolded, const unsigned int hexColor, const int windowWidth, const int windowHeight) {
    //const std::clock_t begin = clock();
    text = rawText;
    x = rawX;
    y = rawY;
    fontSize = size;
    bold = bolded;
    color = hexColor;

    // html entity decode
    sanitize(text);

    // load font, rasterize, save vertices
    rasterize(rawX, rawY, windowWidth, windowHeight);

    // send to video card
    glGenBuffers(1, &elementBufferObject);
    for (unsigned int i = 0; i < glyphVertices.size(); i++) {
        const Glyph &glyph = glyphs[i];
        const std::unique_ptr<float[]> &glyphVertice = glyphVertices[i];
        vertexArrayObjects.push_back(0);
        vertexBufferObjects.push_back(0);
        glGenVertexArrays(1, &vertexArrayObjects.back());
        glGenBuffers(1, &vertexBufferObjects.back());

        glBindVertexArray(vertexArrayObjects.back());

        // the data array
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects.back());
        glBufferData(GL_ARRAY_BUFFER, ((3 + 4 + 2) * 4) * sizeof(float), glyphVertice.get(), GL_STATIC_DRAW);

        // indexes of the array
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

        // upload texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glyph.textureWidth, glyph.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, glyph.textureData.get());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    //const std::clock_t end = clock();
    //std::cout << "buffering text [" <<  text << "] in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
}

TextComponent::~TextComponent() {
    glDeleteBuffers(1, &elementBufferObject);
    for (unsigned int i = 0; i < vertexArrayObjects.size(); i++) {
        glDeleteVertexArrays(1, &vertexArrayObjects[i]);
        glDeleteBuffers(1, &vertexBufferObjects[i]);
        glDeleteTextures(1, &textures[i]);
    }
}

#define posMac(p) p*9

inline void setVertices(std::unique_ptr<float[]> &vertices, int p, unsigned int color) {
    vertices[posMac(p) + 2] = 0.0f;
    vertices[posMac(p) + 3] = (static_cast<float>((color >> 24) & 0xFF)) / 255;
    vertices[posMac(p) + 4] = (static_cast<float>((color >> 16) & 0xFF)) / 255;
    vertices[posMac(p) + 5] = (static_cast<float>((color >>  8) & 0xFF)) / 255;
    vertices[posMac(p) + 6] = (static_cast<float>((color >>  0) & 0xFF)) / 255;
}

void TextComponent::rasterize(const int rawX, const int rawY, const int windowWidth, const int windowHeight) {
    //const std::clock_t begin = clock();
    const std::shared_ptr<TextRasterizer> textRasterizer=rasterizerCache->loadFont(fontSize, bold);
    unsigned int glyphCount;
    glyphs = textRasterizer->rasterize(text, rawX, rawY, windowWidth, windowHeight, height, glyphCount);
    if (glyphs == nullptr) {
        return;
    }

    glyphVertices.clear();
    for (unsigned int i = 0; i < glyphCount; i++) {
    //for(std::vector<Glyph>::iterator it=glyphs->begin(); it!=glyphs->end(); ++it) {
        const Glyph &glyph = glyphs[i];
        //Glyph &glyph=*it;

        float vx0 = glyph.x0;
        float vy0 = glyph.y0;
        float vx1 = glyph.x1;
        float vy1 = glyph.y1;
        pointToViewport(vx0, vy0, windowWidth, windowHeight);
        pointToViewport(vx1, vy1, windowWidth, windowHeight);

        std::unique_ptr<float[]> vertices = std::make_unique<float[]>(36);
        vertices[posMac(0) + 0] = vx0;
        vertices[posMac(0) + 1] = vy0;
        setVertices(vertices, 0, color);
        vertices[posMac(0) + 7] = glyph.s0;
        vertices[posMac(0) + 8] = glyph.t0;

        vertices[posMac(1) + 0] = vx0;
        vertices[posMac(1) + 1] = vy1;
        setVertices(vertices, 1, color);
        vertices[posMac(1) + 7] = glyph.s0;
        vertices[posMac(1) + 8] = glyph.t1;

        vertices[posMac(2) + 0] = vx1;
        vertices[posMac(2) + 1] = vy1;
        setVertices(vertices, 2, color);
        vertices[posMac(2) + 7] = glyph.s1;
        vertices[posMac(2) + 8] = glyph.t1;

        vertices[posMac(3) + 0] = vx1;
        vertices[posMac(3) + 1] = vy0;
        setVertices(vertices, 3, color);
        vertices[posMac(3) + 7] = glyph.s1;
        vertices[posMac(3) + 8] = glyph.t0;

        glyphVertices.push_back(std::move(vertices));
    }
    //const std::clock_t end = clock();
    //std::cout << "rasterize text [" <<  text << "] in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
}

void TextComponent::render() {
    //std::cout << "TextComponent::render [" << text << "]" << std::endl;
    if (verticesDirty) {
        // only called on resize
        //const std::clock_t begin = clock();
        for (unsigned int i = 0; i < vertexBufferObjects.size(); i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[i]);
            glBufferData(GL_ARRAY_BUFFER, ((3 + 4 + 2) * 4) * sizeof(float), glyphVertices[i].get(), GL_STATIC_DRAW);
        }
        verticesDirty = false;
        //const std::clock_t end = clock();
        //std::cout << "undirty TextComponent render [" <<  text << "] in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
    }
    for (unsigned int i = vertexArrayObjects.size(); i > 0; i--) {
        glBindVertexArray(vertexArrayObjects[i - 1]);
        glBindTexture(GL_TEXTURE_2D, textures[i - 1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}

void TextComponent::resize(const int rawX, const int rawY, const int windowWidth, const int windowHeight) {
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
