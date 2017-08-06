#include "InputComponent.h"
#include <iostream>
#include "../opengl/Window.h"
#include "../text/TextRasterizerCache.h"

extern TextRasterizerCache *rasterizerCache;

// : BoxComponent(rawX, rawY, rawWidth, rawHeight, passedWindowWidth, passedWindowHeight)
InputComponent::InputComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int passedWindowWidth, const int passedWindowHeight) {

    //std::cout << "InputComponent::InputComponent - data" << std::endl;
    //std::cout << "InputComponent::InputComponent - window: " << windowWidth << "x" << windowHeight << " passed " << passedWindowWidth << "x" << passedWindowHeight << std::endl;
    
    boundToPage = true;
    
    // set up state
    windowWidth = passedWindowWidth;
    windowHeight = passedWindowHeight;
    width = rawWidth;
    height = rawHeight;
    x = rawX;
    y = rawY;
    
    // copy initial state
    initialX = x;
    initialY = y;
    initialWidth = width;
    initialHeight = height;
    initialWindowWidth = windowWidth;
    initialWindowHeight = windowHeight;
    
    for (int i = 0; i < 3; i++) {
        data[0][0][i] = 0xf0; // set RGB color
    }
    data[0][0][3] = 0xff; // set alpha
    
    float vx = rawX;
    float vy = rawY;
    //std::cout << "placing box at " << (int)vx << "x" << (int)vy << " size: " << (int)rawWidth << "x" << (int)rawHeight << std::endl;
    float vWidth = rawWidth;
    float vHeight = rawHeight;
    pointToViewport(vx, vy);
    
    // converts 512 to 1 and 1 to 2
    //std::cout << "vWidth before: " << (int)vWidth << std::endl;
    distanceToViewport(vWidth, vHeight);
    //std::cout << "vWidth after: " << (int)vWidth << std::endl;
    
    //std::cout << "placing box at GL " << (int)vx << "x" << (int)vy << " size: " << (int)(vWidth*10000) << "x" << (int)(vHeight*10000) << std::endl;
    
    vertices[(0 * 5) + 0] = vx;
    vertices[(0 * 5) + 1] = vy + vHeight;
    vertices[(1 * 5) + 0] = vx + vWidth;
    vertices[(1 * 5) + 1] = vy + vHeight;
    vertices[(2 * 5) + 0] = vx + vWidth;
    vertices[(2 * 5) + 1] = vy;
    vertices[(3 * 5) + 0] = vx;
    vertices[(3 * 5) + 1] = vy;
    
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);
    
    glBindVertexArray(vertexArrayObject);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindVertexArray(0); // protect what we created against any further modification
}

void InputComponent::render() {
    //std::cout << "InputComponent::render" << std::endl;
    GLenum glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "InputComponent::render - start not ok: " << glErr << std::endl;
    }
    if (verticesDirty) {
        //std::cout << "BoxComponent::render - update dirty vertex" << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "InputComponent::render - glBindBuffer not ok: " << glErr << std::endl;
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "InputComponent::render - glBufferData not ok: " << glErr << std::endl;
        }
        verticesDirty = false;
    }
    glBindVertexArray(vertexArrayObject);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "InputComponent::render - glBindVertexArray not ok: " << glErr << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "InputComponent::render - glBindTexture not ok: " << glErr << std::endl;
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "InputComponent::render - glDrawElements not ok: " << glErr << std::endl;
    }
    glBindVertexArray(0);
    // can actuall delete vertices here
    
    if (userInputText) {
        glUseProgram(window->fontProgram);
        //std::cout << "rendering some text" << std::endl;
        userInputText->render();
        glUseProgram(window->textureProgram);
    }
}

void InputComponent::resize(const int passedWindowWidth, const int passedWindowHeight) {
    //std::cout << "InputComponent::resize" << std::endl;
    //std::cout << "InputComponent::resize - rasterizing at " << (int)x << "x" << (int)y << " size: " << (int)width << "x" << (int)height << std::endl;
    
    // maybe already done at component::resize
    // set up state
    windowWidth = passedWindowWidth;
    windowHeight = passedWindowHeight;
    
    // turning this off breaks coordinates
    boundToPage = false;
    
    for (int i = 0; i < 3; i++) {
        data[0][0][i] = 0xf0; // set RGB color
    }
    data[0][0][3] = 0xff; // set alpha
    
    float vx = x;
    float vy = y;
    float vx1 = x + width;
    float vy1 = y - height;
    /*
    std::cout << "placing box at " << (int)vx << "," << (int)vy << " size: " << (int)width << "x" << (int)height << " v1: " << (int)vx1 << "," << (int)vy1 << std::endl;
    //float vWidth = width;
    //float vHeight = height;
    boundToPage = true;
    pointToViewport(vx, vy);
    pointToViewport(vx1, vy1);
    std::cout << "TRUE placing box at GL v: " << vx << "," << vy << " v1: " << vx1 << "," << vy1 << std::endl;
    
    vx = x;
    vy = y;
    vx1 = x + width;
    vy1 = y - height;
    boundToPage = false;
     */
    pointToViewport(vx, vy);
    pointToViewport(vx1, vy1);
    //std::cout << "FALSE placing box at GL v: " << vx << "," << vy << " v1: " << vx1 << "," << vy1 << std::endl;
    
    // converts 512 to 1 and 1 to 2
    //std::cout << "vWidth before: " << (int)vWidth << std::endl;
    //distanceToViewport(vWidth, vHeight);
    //std::cout << "vWidth after: " << (int)vWidth << std::endl;
    
    
    vertices[(0 * 5) + 0] = vx;
    vertices[(0 * 5) + 1] = vy1;

    vertices[(1 * 5) + 0] = vx1;
    vertices[(1 * 5) + 1] = vy1;
    
    vertices[(2 * 5) + 0] = vx1;
    vertices[(2 * 5) + 1] = vy;
    
    vertices[(3 * 5) + 0] = vx;
    vertices[(3 * 5) + 1] = vy;
    
    glBindVertexArray(vertexArrayObject);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
   
    glBindVertexArray(0); // protect what we created against any further modification
}

void InputComponent::addChar(char c) {
    value+=c;
    //std::cout << "input value is now: " << value << std::endl;
    if (userInputText) {
        delete userInputText;
    }
    //std::cout << "InputComponent::addChar - at " << (int)x << "," << (int)y << std::endl;
    updateText();
}
void InputComponent::backSpace() {
    value=value.substr(0, value.length() - 1);
    if (userInputText) {
        delete userInputText;
    }
    //std::cout << "InputComponent::addChar - at " << (int)x << "," << (int)y << std::endl;
    updateText();
}

void InputComponent::updateText() {
    userInputText=new TextComponent(value, 0, 0, 12, false, 0x000000FF, windowWidth, windowHeight);
    userInputText->x = x;
    userInputText->y = y;
    // 125 pixels width
    // but first we need to know how wide the text is
    const std::shared_ptr<TextRasterizer> textRasterizer=rasterizerCache->loadFont(12, false); // fontSize, bold
    rasterizationRequest request;
    request.text = value;
    request.startX = x;
    request.availableWidth = windowWidth;
    request.sourceStartX = 0;
    request.sourceStartY = 0;
    request.noWrap = true;
    std::unique_ptr<std::pair<int, int>> textInfo=textRasterizer->size(request);
    if (textInfo.get() == nullptr) {
        std::cout << "InputComponent::updateText couldn't estimate value[" << value << "] size" << std::endl;
        return;
    }
    int estWidth = std::get<0>(*textInfo.get());
    //int estHeight = std::get<1>(*textInfo.get());
    userInputText->rasterStartX = 0;
    userInputText->rasterStartY = 0;
    //std::cout << "estWidth: " << estWidth << " width: " << static_cast<int>(width) << std::endl;
    if (estWidth > width) {
        //std::cout << "scrolling text" << std::endl;
        userInputText->rasterStartX = estWidth - width;
    }
    userInputText->noWrap = true;
    // FIXME: not really 125, if we're x>windowWidth-125
    // why does changing the width mess shit up?
    //std::cout << "our width: " << static_cast<int>(width) << " windowWidth: " << windowWidth << std::endl;
    userInputText->resize(windowWidth, windowHeight, 125); // need to make sure there's a texture
    window->renderDirty = true;
}
