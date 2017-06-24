#ifndef WINDOW_H
#define WINDOW_H

#include "Box.h"
#include "Text.h"
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
    const GLuint compileShader(const GLenum shaderType, const char *shaderSource) const;
    const GLuint compileProgram(const GLuint vertexShader, const GLuint fragmentShader) const;
    void render() const;
    GLFWwindow *window;
    int windowWidth;
    int windowHeight;
    std::vector<std::unique_ptr<Box>> boxes;
    std::vector<std::unique_ptr<Text>> texts;
};

#endif
