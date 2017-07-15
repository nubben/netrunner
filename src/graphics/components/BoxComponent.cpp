#include "BoxComponent.h"
#include <cmath>

BoxComponent::BoxComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int windowWidth, const int windowHeight) {
    x = rawX;
    y = rawY;
    width = rawWidth;
    height = rawHeight;

    for (int py = 0; py < 1024; py++) {
        for (int px = 0; px < 1024; px++) {
            for (int i = 0; i < 3; i++) {
                data[1023 - py][px][i] = 0x88;
            }
            data[1023 - py][px][3] = 0xff;
        }
    }

    float vx = rawX;
    float vy = rawY;
    float vWidth = rawWidth;
    float vHeight = rawHeight;
    pointToViewport(vx, vy, windowWidth, windowHeight);
    distanceToViewport(vWidth, vHeight, windowWidth, windowHeight);

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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

BoxComponent::~BoxComponent() {
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &elementBufferObject);
    glDeleteTextures(1, &texture);
}

void BoxComponent::render() {
    if (verticesDirty) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        verticesDirty = false;
    }
    glBindVertexArray(vertexArrayObject);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void BoxComponent::resize(const int windowWidth, const int windowHeight) {
    float vx = x;
    float vy = y;
    float vWidth = width;
    float vHeight = height;
    pointToViewport(vx, vy, windowWidth, windowHeight);
    distanceToViewport(vWidth, vHeight, windowWidth, windowHeight);

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

void BoxComponent::pointToViewport(float &rawX, float &rawY, const int windowWidth, const int windowHeight) const {
    if (rawX < 0) {
        rawX += windowWidth;
    }
    if (rawY < 0) {
        rawY += windowHeight;
    }
    if (rawX > 1) {
        rawX /= windowWidth;
    }
    if (rawY > 1) {
        rawY /= windowHeight;
    }
    rawX = (rawX * 2) - 1;
    rawY = (rawY * 2) - 1;
}

void BoxComponent::distanceToViewport(float &rawX, float &rawY, const int windowWidth, const int windowHeight) const {
    if (std::abs(rawX) > 1) {
        rawX /= windowWidth;
    }
    if (std::abs(rawY) > 1) {
        rawY /= windowHeight;
    }
    rawX *= 2;
    rawY *= 2;
}
