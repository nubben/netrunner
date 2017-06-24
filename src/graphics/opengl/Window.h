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
    bool initGLEW();
    bool initGL();
    GLuint compileShader(GLenum shaderType, const char *shaderSource);
    GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader);
    void render();
    GLFWwindow *window;
    int windowWidth;
    int windowHeight;
    std::vector<std::unique_ptr<Box>> boxes;
    std::vector<std::unique_ptr<Text>> texts;
};

#endif
