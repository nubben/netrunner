#ifndef WINDOW_H
#define WINDOW_H

#include "../components/AnimeComponent.h"
#include "../components/BoxComponent.h"
#include "../components/TextComponent.h"
#include "../components/Component.h"
#include "../components/ComponentBuilder.h"
#include "../../html/Node.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <algorithm>
#include "../../networking/HTTPResponse.h"
#include "../../URL.h"

class Window {
private:
public:
    GLuint fontProgram = 0;
    GLuint textureProgram = 0;
    ~Window();
    bool init();
    bool initGLFW();
    bool initGLEW() const;
    bool initGL();
    GLuint compileShader(const GLenum shaderType, const char *shaderSource) const;
    GLuint compileProgram(const GLuint vertexShader, const GLuint fragmentShader) const;
    void render();
    void setDOM(const std::shared_ptr<Node> rootNode);
    void createComponentTree(const std::shared_ptr<Node> rootNode, const std::shared_ptr<Component> &parentComponent);
    void printComponentTree(const std::shared_ptr<Component> &component, int depth);
    void renderComponents(std::shared_ptr<Component> component);
    void renderBoxComponents(std::shared_ptr<Component> component);
    void resizeComponentTree(const std::shared_ptr<Component> &component, const int windowWidth, const int windowHeight);
    std::shared_ptr<Component> searchComponentTree(const std::shared_ptr<Component> &component, const int x, const int y);
    void navTo(std::string url);
    // properties
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
    std::shared_ptr<Node> domRootNode = nullptr;
    bool domDirty = false;
    bool renderDirty = false;
    ComponentBuilder componentBuilder;
    //std::vector<std::unique_ptr<BoxComponent>> boxComponents;
    std::shared_ptr<Component> rootComponent = std::make_shared<Component>();
    // could break these out in some sort of cursor class
    // to minimize dependencies
    std::shared_ptr<Component> focusedComponent = nullptr;
    std::shared_ptr<Component> hoverComponent = nullptr;
    double cursorX = 0;
    double cursorY = 0;
    unsigned int delayResize = 0;
    GLFWcursor* cursorHand;
    GLFWcursor* cursorArrow;
    GLFWcursor* cursorIbeam;
    URL currentURL;
};

bool setWindowContent(URL const& url);
void handleRequest(const HTTPResponse &response);

extern const std::unique_ptr<Window> window;

#endif
