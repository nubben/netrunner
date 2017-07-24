#include "Window.h"
#include "shaders/gen/FontShader.h"
#include "shaders/gen/TextureShader.h"
#include "../../html/TagNode.h"
#include "../../html/TextNode.h"
#include <cmath>
#include <ctime>
#include <iostream>

void updateComponentSize(const std::shared_ptr<Component> &component);
void repositionComponent(const std::shared_ptr<Component> &component);

Window::~Window() {
    glDeleteProgram(fontProgram);
    glDeleteProgram(textureProgram);

    glfwTerminate();
}

bool Window::init() {
    if (!initGLFW()) {
        return false;
    }
    if (!initGLEW()) {
        return false;
    }
    initGL();

    //UI
    boxComponents.push_back(std::make_unique<BoxComponent>(0.0f, 1.0f, 1.0f, -64, windowWidth, windowHeight));

    //Mascot
    boxComponents.push_back(std::make_unique<AnimeComponent>(-512, 0.0f, 512, 512, windowWidth, windowHeight));

    rootComponent->y = 900;

    return true;
}

bool Window::initGLFW() {
    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "NetRunner", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cout << "Could not create window" << std::endl;
        return false;
    }
    glfwSetWindowUserPointer(window, this);
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *win, int width, int height) {
        glViewport(0, 0, width, height);
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        thiz->windowWidth = width;
        thiz->windowHeight = height;
        for (const std::unique_ptr<BoxComponent> &boxComponent : thiz->boxComponents) {
            boxComponent->resize(width, height);
        }
        std::cout << "resizing" << std::endl;
        thiz->resizeComponentTree(thiz->rootComponent, width, height);
        thiz->printComponentTree(thiz->rootComponent, 0);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *win, double xPos, double yPos) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        thiz->cursorX = xPos;
        thiz->cursorY = yPos;
    });
    glfwSetScrollCallback(window, [](GLFWwindow *win, double xOffset, double yOffset) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        thiz->transformMatrix[13] += -yOffset * 0.1;
        thiz->transformMatrixDirty = true;
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *win, int button, int action, int mods) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            std::shared_ptr<Component> clickedComponent = thiz->searchComponentTree(thiz->rootComponent, thiz->cursorX, (thiz->windowHeight - thiz->cursorY) + ((-thiz->transformMatrix[13] / 2) * thiz->windowHeight));
            if (clickedComponent) {
                if (clickedComponent->onClick) {
                    clickedComponent->onClick();
                }
            }
        }
    });
    glfwMakeContextCurrent(window);

    return true;
}

bool Window::initGLEW() const {
    glewExperimental = GL_TRUE;
    const GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Could not initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    return true;
}

bool Window::initGL() {
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Version: " << version << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.8f, 0.8f, 0.8f, 0.8f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const GLuint fontVertexShader = compileShader(GL_VERTEX_SHADER, fontVertexShaderSource);
    const GLuint fontFragmentShader = compileShader(GL_FRAGMENT_SHADER, fontFragmentShaderSource);
    fontProgram = compileProgram(fontVertexShader, fontFragmentShader);
    glDeleteShader(fontVertexShader);
    glDeleteShader(fontFragmentShader);

    const GLuint textureVertexShader = compileShader(GL_VERTEX_SHADER, textureVertexShaderSource);
    const GLuint textureFragmentShader = compileShader(GL_FRAGMENT_SHADER, textureFragmentShaderSource);
    textureProgram = compileProgram(textureVertexShader, textureFragmentShader);
    glDeleteShader(textureVertexShader);
    glDeleteShader(textureFragmentShader);

    std::cout << "OpenGL setup" << std::endl;

    return true;
}

GLuint Window::compileShader(const GLenum shaderType, const char *shaderSource) const {
    const GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cout << "Could not compile shader\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint Window::compileProgram(const GLuint vertexShader, const GLuint fragmentShader) const {
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "Could not compile program\n" << infoLog << std::endl;
    }

    return program;
}

void Window::render() {
    if (domDirty) {
        const std::clock_t begin = clock();
        createComponentTree(domRootNode, rootComponent);
        const std::clock_t end = clock();
        std::cout << "Parsed dom in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
        printComponentTree(rootComponent, 0);
        domDirty = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(textureProgram);
    for (const std::unique_ptr<BoxComponent> &boxComponent : boxComponents) {
        boxComponent->render();
    }
    // it's quick but done on scroll
    glUseProgram(fontProgram);
    if (transformMatrixDirty) {
        //const std::clock_t begin = clock();
        GLint transformLocation = glGetUniformLocation(fontProgram, "transform");
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, transformMatrix);
        //const std::clock_t end = clock();
        //std::cout << "Updated font matrix in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
        transformMatrixDirty = false;
    }
    renderComponents(rootComponent);
    glfwPollEvents();
    glfwSwapBuffers(window);
}

void Window::setDOM(const std::shared_ptr<Node> rootNode) {
    domRootNode = rootNode;
    domDirty = true;
}

// reposition component based on parent position
void repositionComponent(const std::shared_ptr<Component> &component) {
    std::shared_ptr<Component> prev = nullptr;
    int x = 0, y = 0;
    if (component->parent) {
        for (std::shared_ptr<Component> child : component->parent->children) {
            if (child == component) {
                break;
            }
            prev = child;
        }
        x = component->parent->x;
        y = component->parent->y;
    } else {
        std::cout << "no parent" << std::endl;
    }
    if (prev) {
        TextComponent *textComponent = dynamic_cast<TextComponent*>(prev.get());
        // 2nd or last
        if (prev->isInline) {
            // last was inline
            if (component->isInline) {
                x = prev->x + prev->width;
                y = prev->y; // keep on same line
                // unless it wraps
                /*
                if (textComponent) {
                    if (textComponent->inLineYPos) {
                        // did wrap
                        std::cout << "previous line at " << y << " wrapped at " << textComponent->inLineYPos << std::endl;
                    } else {
                        // previous line didn't wrap
                    }
                }
                */
            } else {
                // we're block
                y = prev->y - prev->height;
            }
        } else {
            // last was block
            y = prev->y - prev->height;
        }
    } else {
        // first, there will be no width to add
    }
    //std::cout << "moving component to " << (int)x << "x" << (int)y << std::endl;
    component->x = x;
    component->y = y;
}

// measure current, apply changes to parent
void updateComponentSize(const std::shared_ptr<Component> &component) {
    unsigned int maxWidth = 0; // float?

    // find max width of all siblings
    for (std::shared_ptr<Component> child : component->parent->children) {
        maxWidth = std::max(maxWidth, static_cast<unsigned int>(child->width));
    }
    //std::cout << "new size " << maxWidth << "x" << maxHeight << std::endl;
    
    // make deltas by comparing max to delta
    //int widthIncrease = maxWidth - component->width;
    //int heightIncrease = maxHeight - component->height;
    //std::cout << "widthIncrease: " << widthIncrease << std::endl;
    
    // this works, not sure why, but it is the height delta
    // and if we aggregate all the height deltas together, we get the correct height
    int heightIncrease = (component->parent->y - component->parent->height) - (component->y - component->height);
    
    // apply adjustment to all parents (instead of recalculating each)
    for (std::shared_ptr<Component> parent = component->parent; parent != nullptr; parent = parent->parent) {
        //parent->width += widthIncrease;
        parent->height += heightIncrease;
        parent->width = std::max(static_cast<unsigned int>(parent->width), maxWidth);
        //parent->height = std::max(static_cast<unsigned int>(parent->height), maxHeight);
    }
}

void Window::createComponentTree(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent) {
    std::shared_ptr<Component> component = componentBuilder.build(node, parentComponent, windowWidth, windowHeight);

    // set parent of the newly created component
    component->parent = parentComponent;
    if (parentComponent) {
        // add new component as child to parent
        parentComponent->children.push_back(component);
    }
    
    // create children elements
    for (std::shared_ptr<Node> child : node->children) {
        createComponentTree(child, component);
    }
    // update parents
    updateComponentSize(component);
}

void Window::printComponentTree(const std::shared_ptr<Component> &component, int depth) {
    for (int i = 0; i < depth; i++) {
        std::cout << '\t';
    }
    TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
    if (textComponent) {
        std::cout << std::fixed << "X: " << textComponent->x << " Y: " << textComponent->y << " WIDTH: " << textComponent->width << " HEIGHT: " << textComponent->height << " INLINE: " << textComponent->isInline << " TEXT: " << textComponent->text << std::endl;
    }
    else {
        std::cout << std::fixed << "X: " << component->x << " Y: " << component->y << " WIDTH: " << component->width << " HEIGHT: " << component->height << " INLINE: " << component->isInline << std::endl;
    }
    for (std::shared_ptr<Component> child : component->children) {
        printComponentTree(child, depth + 1);
    }
}

void Window::renderComponents(std::shared_ptr<Component> component) {
    TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
    if (textComponent) {
        textComponent->render();
    }
    if (component->children.empty()) {
        return;
    }
    for (std::shared_ptr<Component> &child : component->children) {
        renderComponents(child);
    }
}

// fuck not a tree, but left to right serialized instructions
void Window::resizeComponentTree(const std::shared_ptr<Component> &component, const int width, const int height) {
    // we could calculate the position before we determine text or not tbh

    TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
    // if dynamic cast fails?
    // is all text considered inline?
    if (textComponent) {
        // FIXME: what if no parent
        repositionComponent(component);
        textComponent->resize(component->x, component->y, width, height);
        updateComponentSize(component);
    }
    else {
        //std::cout << "inlined: " << component->isInline << std::endl;
        // if we have a parent, change our position based on parent's position
        if (component->parent) {
            component->x = component->parent->x;
            component->y = component->parent->y - component->parent->height;
            repositionComponent(component);
        }
        // we're renderless so we have no w/h
        component->width = 0;
        component->height = 0;
    }
    for (std::shared_ptr<Component> child : component->children) {
        resizeComponentTree(child, width, height);
    }
}

// used for picking
std::shared_ptr<Component> Window::searchComponentTree(const std::shared_ptr<Component> &component, const int x, const int y) {
    if (component->children.empty()) {
        if (component->y > y && component->y - component->height < y) {
            if (component->x < x && component->x + component->width > x) {
                return component;
            }
        }
    }
    else {
        for (std::shared_ptr<Component> child : component->children) {
            std::shared_ptr<Component> found = searchComponentTree(child, x, y);
            if (found) {
                return found;
            }
        }
    }
    return nullptr;
}
