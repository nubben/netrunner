#include "Box.h"
#include "../../../anime.h"
#include <cmath>

Box::Box(const float x, const float y, const float width, const float height, const int windowWidth, const int windowHeight) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    if (width == 1024) {
        for (int y = 0; y < 1024; y++) {
            for (int x = 0; x < 1024; x++) {
                for (int i = 0; i < 4; i++) {
                    data[1023 - y][x][i] = anime.pixel_data[((x * 4) + (y * 4 * 1024)) + i];
                }
            }
        }
    }
    else {
        for (int y = 0; y < 1024; y++) {
            for (int x = 0; x < 1024; x++) {
                for (int i = 0; i < 3; i++) {
                    data[1023 - y][x][i] = 0x88;
                }
                data[1023 - y][x][3] = 0xff;
            }
        }
    }

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

    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Box::~Box() {
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &elementBufferObject);
    glDeleteTextures(1, &texture);
}

void Box::render() {
    if (verticesDirty) {
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        verticesDirty = false;
    }
    glBindVertexArray(vertexArrayObject);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Box::resize(const int windowWidth, const int windowHeight) {
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

void Box::pointToViewport(float &x, float &y, const int windowWidth, const int windowHeight) const {
    if (x < 0) {
        x += windowWidth;
    }
    if (y < 0) {
        y += windowHeight;
    }
    if (x > 1) {
        x /= windowWidth;
    }
    if (y > 1) {
        y /= windowHeight;
    }
    x = (x * 2) - 1;
    y = (y * 2) - 1;
}

void Box::distanceToViewport(float &x, float &y, const int windowWidth, const int windowHeight) const {
    if (std::abs(x) > 1) {
        x /= windowWidth;
    }
    if (std::abs(y) > 1) {
        y /= windowHeight;
    }
    x *= 2;
    y *= 2;
}
