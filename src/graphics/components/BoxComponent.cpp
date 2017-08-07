#include "BoxComponent.h"
#include <cmath>
#include <iostream>

BoxComponent::BoxComponent() {
    //std::cout << "BoxComponent::BoxComponent - empty" << std::endl;
}

BoxComponent::BoxComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int passedWindowWidth, const int passedWindowHeight) {
    //std::cout << "BoxComponent::BoxComponent - data" << std::endl;
    //std::cout << "BoxComponent::BoxComponent - window: " << windowWidth << "x" << windowHeight << " passed " << passedWindowWidth << "x" << passedWindowHeight << std::endl;
    
    boundToPage = false;
    
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
        data[0][0][i] = 0x88; // set RGB color
    }
    data[0][0][3] = 0xff; // set alpha

    float vx = rawX;
    float vy = rawY;
    //std::cout << "placing box at " << (int)vx << "x" << (int)vy << " size: " << (int)rawWidth << "x" << (int)rawHeight << std::endl;
    float vWidth = rawWidth;
    float vHeight = rawHeight;
    pointToViewport(vx, vy);
    
    /*
    // if has rawWidth > 1
    if (std::abs(vWidth) > 1) {
        // try to reduce vWidth < 1
        vWidth /= windowWidth;
    }
    // if has rawWidth > 1
    if (std::abs(vWidth) > 1) {
        // try to reduce vWidth < 1
        vWidth /= windowHeight;
    }
    // double w/h
    vWidth *= 2;
    vWidth *= 2;
    */
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

BoxComponent::~BoxComponent() {
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &elementBufferObject);
    glDeleteTextures(1, &texture);
}

void BoxComponent::render() {
    //std::cout << "BoxComponent::render" << std::endl;
    GLenum glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "BoxComponent::render - start not ok: " << glErr << std::endl;
    }
    if (verticesDirty) {
        //std::cout << "BoxComponent::render - update dirty vertex" << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "BoxComponent::render - glBindBuffer not ok: " << glErr << std::endl;
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "BoxComponent::render - glBufferData not ok: " << glErr << std::endl;
        }
        verticesDirty = false;
    }
    glBindVertexArray(vertexArrayObject);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "BoxComponent::render - glBindVertexArray not ok: " << glErr << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "BoxComponent::render - glBindTexture not ok: " << glErr << std::endl;
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "BoxComponent::render - glDrawElements not ok: " << glErr << std::endl;
    }
    glBindVertexArray(0);
    // can actuall delete vertices here
}

/*
void BoxComponent::resize(const int passedWindowWidth, const int passedWindowHeight) {

    std::cout << "BoxComponent::resize" << std::endl;
    windowWidth = passedWindowWidth;
    windowHeight = passedWindowHeight;
    
    if (boundToPage) {
        std::cout << "BoxComponent::resize - boundToPage doing nothing" << std::endl;
    } else {
        
        // figure out our percentages
        // 0 / WW = 0
        // 768 / 1024 = 75%
        float xPer = initialX / (float)initialWindowWidth;
        float yPer = initialY / (float)initialWindowHeight;
        // adjust width
        float wPer = initialWidth / (float)initialWindowWidth;
        //float hPer = initialHeight / (float)initialWindowHeight;
        
        float vx = xPer * windowWidth;
        float vy = yPer * windowHeight;
        
        // nah I don't think we want things to stretch
        //float vWidth = wPer * windowWidth;
        //float vHeight = hPer * windowHeight;
        
        // ugh these just need to be relaid out
        //float vWidth = width;
        float vWidth = wPer * windowWidth;
        float vHeight = height;
        
        //std::cout << "initial: " << initialX << "x" << initialY << " size: " << initialWidth << "x" << initialHeight << " window size: " << initialWindowWidth << "x" << initialWindowHeight << std::endl;

        //std::cout << "scaled to: " << (int)vx << "x" << (int)vy << " size: " << (int)vWidth << "x" << (int)vHeight << " window size: " << windowWidth << "x" << windowHeight << std::endl;

        
        pointToViewport(vx, vy);
        distanceToViewport(vWidth, vHeight);
        
        vertices[(0 * 5) + 0] = vx;
        vertices[(0 * 5) + 1] = vy + vHeight;
        vertices[(1 * 5) + 0] = vx + vWidth;
        vertices[(1 * 5) + 1] = vy + vHeight;
        vertices[(2 * 5) + 0] = vx + vWidth;
        vertices[(2 * 5) + 1] = vy;
        vertices[(3 * 5) + 0] = vx;
        vertices[(3 * 5) + 1] = vy;
        
        verticesDirty = true;
    }
}
*/
