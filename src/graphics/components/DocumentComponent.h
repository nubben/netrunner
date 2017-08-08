#ifndef DOCUMENTCOMPONENT_H
#define DOCUMENTCOMPONENT_H

#include <GL/glew.h>
#include "Component.h"
#include "ComponentBuilder.h"
#include "../opengl/Window.h"
#include "../../html/Node.h"
#include "../../URL.h"
#include "../../networking/HTTPResponse.h"

class DocumentComponent : public Component {
public:
    DocumentComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int passedWindowWidth, const int passedWindowHeight);
    void printComponentTree(const std::shared_ptr<Component> &component, int depth);
    void render();
    void createComponentTree(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent);
    void renderComponents(std::shared_ptr<Component> component);
    void renderBoxComponents(std::shared_ptr<Component> component);
    std::shared_ptr<Component> searchComponentTree(const std::shared_ptr<Component> &component, const int x, const int y);
    void navTo(const std::string url);
    std::shared_ptr<Node> domRootNode = nullptr;
    ComponentBuilder componentBuilder;
    std::shared_ptr<Component> rootComponent = std::make_shared<Component>();
    URL currentURL;
    bool domDirty = false;
    bool renderDirty = false;
    // we'll need a way to pass events down and up
    // also how do we handle scroll?
    // we'll need searchComponentTree for picking
    Window *win;
    std::shared_ptr<Component> hoverComponent = nullptr;
    std::shared_ptr<Component> focusedComponent = nullptr;
    int scrollY = 0;
    int scrollHeight = 0;
};

//bool setWindowContent(URL const& url);
//void handleRequest(const HTTPResponse &response);

extern const std::unique_ptr<Window> window;

#endif
