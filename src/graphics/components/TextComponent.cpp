#include "TextComponent.h"
#include <iostream>
#include <ctime>

#define posMac(p) (p * (3 + 4 + 2)) // 3 positions + 4 color channels + 2 S&T (texture mapping)

extern TextRasterizerCache *rasterizerCache;

TextComponent::TextComponent(const std::string &rawText, const int rawX, const int rawY, const unsigned int size, const bool bolded, const unsigned int hexColor, const int passedWindowWidth, const int passedWindowHeight) {

    windowWidth = passedWindowWidth;
    windowHeight = passedWindowHeight;
    
    //const std::clock_t begin = clock();
    text = rawText;
    //x = rawX;
    //y = rawY;
    x = 0;
    y = 0;
    fontSize = size;
    bold = bolded;
    color = hexColor;

    // html entity decode
    sanitize(text);
    
    
    // load font, rasterize, save vertices
    //rasterize(x, y, windowWidth, windowHeight);

    // send to video card
    glGenBuffers(1, &elementBufferObject);
    //for (unsigned int i = 0; i < glyphVertices.size(); i++) {
    //int i = 0;
        //const Glyph &glyph = glyphs[i];
        //const std::unique_ptr<float[]> &glyphVertice = glyphVertices[i];
    
        vertexArrayObjects.push_back(0);
        vertexBufferObjects.push_back(0);

        glGenVertexArrays(1, &vertexArrayObjects.back());
        glGenBuffers(1, &vertexBufferObjects.back());
    
        glBindVertexArray(vertexArrayObjects.back());

        // the data array
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects.back());
        std::unique_ptr<float[]> vertices = std::make_unique<float[]>(36); // upload garbage for now
        glBufferData(GL_ARRAY_BUFFER, ((3 + 4 + 2) * 4) * sizeof(float), vertices.get(), GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, ((3 + 4 + 2) * 4) * sizeof(float), glyphVertice.get(), GL_STATIC_DRAW);

        // indexes of the array
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (3 + 4 + 2) * sizeof(float), reinterpret_cast<void*>(7 * sizeof(float)));
        glEnableVertexAttribArray(2);
    
        /*
        textures.push_back(0);
        glGenTextures(1, &textures.back());
        glBindTexture(GL_TEXTURE_2D, textures.back());
         */

        // upload texture
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glyph.textureWidth, glyph.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, glyph.textureData.get());
        //glGenerateMipmap(GL_TEXTURE_2D);

    //}
    //const std::clock_t end = clock();
    //std::cout << "buffering text [" <<  text << "] in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
}

TextComponent::~TextComponent() {
    glDeleteBuffers(1, &elementBufferObject);
    for (unsigned int i = 0; i < vertexArrayObjects.size(); i++) {
        glDeleteVertexArrays(1, &vertexArrayObjects[i]);
        glDeleteBuffers(1, &vertexBufferObjects[i]);
        if (textures.size()>i) {
            glDeleteTextures(1, &textures[i]);
        }
    }
}

inline void setVerticesColor(std::unique_ptr<float[]> &vertices, int p, unsigned int color) {
    vertices[static_cast<size_t>(posMac(p) + 2)] = 0.0f;
    vertices[static_cast<size_t>(posMac(p) + 3)] = (static_cast<float>((color >> 24) & 0xFF)) / 255;
    vertices[static_cast<size_t>(posMac(p) + 4)] = (static_cast<float>((color >> 16) & 0xFF)) / 255;
    vertices[static_cast<size_t>(posMac(p) + 5)] = (static_cast<float>((color >>  8) & 0xFF)) / 255;
    vertices[static_cast<size_t>(posMac(p) + 6)] = (static_cast<float>((color >>  0) & 0xFF)) / 255;
}

void TextComponent::rasterize(const int rawX, const int rawY) {
    //const std::clock_t begin = clock();
    const std::shared_ptr<TextRasterizer> textRasterizer=rasterizerCache->loadFont(fontSize, bold);
    //unsigned int glyphCount;
    // we need to know how much width we have between x and windowWidth
    // and preferrable where the inline starts
    // my inline start is parent->children adding width from start to me
    // actually the run of all inline before me, starting at parent x as a minimum
    //std::cout << "TextComponent::rasterize pre-height: " << (int)height << std::endl;
    int wrapToX = 0; // windowWidth - rawX;
    //bool wrapped;
    //glyphs = textRasterizer->rasterize(text, rawX, windowWidth, wrapToX, width, height, glyphCount, endingX, endingY, wrapped);
    rasterizationRequest request;
    request.text = text;
    request.startX = rawX;
    request.availableWidth = availableWidth;
    request.sourceStartX = rasterStartX;
    request.sourceStartY = rasterStartY;
    request.noWrap = noWrap;
    std::shared_ptr<rasterizationResponse> response = textRasterizer->rasterize(request);
    if (response.get() == nullptr) {
        std::cout << "TextComponent::rasterize - got nullptr from rasterizer" << std::endl;
        return;
    }
    width = response->width;
    height = response->height;
    endingX = response->endingX;
    endingY = response->endingY;
    
    //std::cout << "TextComponent::rasterize done [" <<  text << "] - start x: " << rawX << " width: " << (int)width << " wrapWidth: " << windowWidth << " wrapTo: " << wrapToX << std::endl;
    //std::cout << "TextComponent::rasterize post-height: " << (int)height << std::endl;
    //if (glyphs == nullptr) {
    //    return;
    //}
    // did we wrap
    int startX = rawX;
    // if we didn't we have a nice little texture starting at rawX potentially up to windowWidth
    // if we did wrap, then we start at wrapToX (0) and we're a big square texture
    if (response->wrapped) {
        //std::cout << "it's wrapped starting at " << wrapToX << std::endl;
        startX = wrapToX;
    }
    //std::cout << "startX: " << startX << std::endl;

    glyphVertices.clear();
    //for (unsigned int i = 0; i < glyphCount; i++) {
    //for(std::vector<Glyph>::iterator it=glyphs->begin(); it!=glyphs->end(); ++it) {
        //const Glyph &glyph = glyphs[i];
        //Glyph &glyph=*it;

        float vx0 = startX;
        float vy0 = response->y0 + rawY;
        //std::cout << "glyph x from: " << (int)glyph.x0 << " to " << (int)glyph.x1 << std::endl;
        float vx1 = startX + (response->x1 - response->x0);
        float vy1 = response->y1 + rawY;
        //std::cout << "textcomponent at " << (int)vx0 << "," << (int)vy0 << " to " << (int)vx1 << "," << (int)vy1 << std::endl;
        
        // convert our local x,y,w,h into actual ogl coords
        
        // map vx0 between -1 and 1
        //vx0 = ((vx0 / windowWidth) * 2) - 1;
        //vy0 = ((vy0 / windowHeight) * 2) - 1;
        
        pointToViewport(vx0, vy0);
        pointToViewport(vx1, vy1);

        //std::cout << "textcomponent at GL" << (int)vx0 << "," << (int)vy0 << " to GL" << (int)vx1 << "," << (int)vy1 << std::endl;
    
        std::unique_ptr<float[]> vertices = std::make_unique<float[]>(36);
        vertices[posMac(0) + 0] = vx0;
        vertices[posMac(0) + 1] = vy0;
        setVerticesColor(vertices, 0, color);
        vertices[posMac(0) + 7] = response->s0;
        vertices[posMac(0) + 8] = response->t0;

        vertices[posMac(1) + 0] = vx0;
        vertices[posMac(1) + 1] = vy1;
        setVerticesColor(vertices, 1, color);
        vertices[posMac(1) + 7] = response->s0;
        vertices[posMac(1) + 8] = response->t1;

        vertices[posMac(2) + 0] = vx1;
        vertices[posMac(2) + 1] = vy1;
        setVerticesColor(vertices, 2, color);
        vertices[posMac(2) + 7] = response->s1;
        vertices[posMac(2) + 8] = response->t1;

        vertices[posMac(3) + 0] = vx1;
        vertices[posMac(3) + 1] = vy0;
        setVerticesColor(vertices, 3, color);
        vertices[posMac(3) + 7] = response->s1;
        vertices[posMac(3) + 8] = response->t0;

        glyphVertices.push_back(std::move(vertices));
    //std::cout << "TextComponent::rasterize - glyphVertices count: " << glyphVertices.size() << std::endl;
    
    textureWidth = static_cast<GLsizei>(response->textureWidth);
    textureHeight = static_cast<GLsizei>(response->textureHeight);
    textureData = std::move(response->textureData);
    //}
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
    // have to have a texture to draw
    if (textures.size()) {
        //std::cout << "has textures" << std::endl;
        for (unsigned long i = vertexArrayObjects.size(); i > 0; i--) {
            glBindVertexArray(vertexArrayObjects[i - 1]); // load vertices
            glBindTexture(GL_TEXTURE_2D, textures[i - 1]); // load texture
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // draw primitives using vertices and texture
        }
    //} else {
        //std::cout << "no textures" << std::endl;
    }
}

// compatible adapter
void TextComponent::resize(const int passedWindowWidth, const int passedWindowHeight) {
    resize(passedWindowWidth, passedWindowHeight, passedWindowWidth);
}

// more detailed control
void TextComponent::resize(const int passedWindowWidth, const int passedWindowHeight, const int passedAvailableWidth) {
    windowWidth = passedWindowWidth;
    windowHeight = passedWindowHeight;
    availableWidth = passedAvailableWidth;
    //std::cout << "TextComponent::resize" << std::endl;
    rasterize(x, y);
    //std::cout << "TextComponent::resize - rasterizing at " << (int)x << "x" << (int)y << " size: " << (int)width << "x" << (int)height << std::endl;

    // make sure we have glyphs
    if (!glyphVertices.size()) {
        std::cout << "TextComponent::resize - no glyphs" << std::endl;
        return;
    }

    // reupload NEW texture to video card
    /*
    const Glyph &glyph = glyphs[0];
    if (!glyphs) {
        std::cout << "TextComponent::resize - glyph points no where" << std::endl;
        return;
    }
    */
    if (!textures.size()) {
        textures.push_back(0);
        glGenTextures(1, &textures.back());
    }
    glBindTexture(GL_TEXTURE_2D, textures.back()); // select texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureData.get()); // update texture
    glGenerateMipmap(GL_TEXTURE_2D);

    verticesDirty = true;
}

void TextComponent::sanitize(std::string &str) {
    size_t found = 0; // position
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
