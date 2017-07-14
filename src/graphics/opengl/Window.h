#ifndef WINDOW_H
#define WINDOW_H

#include "../components/BoxComponent.h"
#include "../components/TextComponent.h"
#include "../components/Component.h"
#include "../components/ComponentBuilder.h"
#include "../../html/Node.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

class Window {
private:
    GLuint fontProgram = 0;
    GLuint textureProgram = 0;
public:
    ~Window();
    bool init();
    bool initGLFW();
    bool initGLEW() const;
    bool initGL();
    GLuint compileShader(const GLenum shaderType, const char *shaderSource) const;
    GLuint compileProgram(const GLuint vertexShader, const GLuint fragmentShader) const;
    void render();
    void setDOM(const std::shared_ptr<Node> rootNode);
    void drawNode(const std::shared_ptr<Node> rootNode);
    void repositionNode(const std::shared_ptr<Node> rootNode);
    float transformMatrix[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    bool transformMatrixDirty = true;
    GLFWwindow *window;
    int windowWidth;
    int windowHeight;
    std::shared_ptr<Node> domRootNode;
    bool domDirty = false;
    ComponentBuilder componentBuilder;
    std::vector<std::unique_ptr<BoxComponent>> boxComponents;
    std::vector<std::unique_ptr<Component>> components;
    int y = 950;
};

#endif
