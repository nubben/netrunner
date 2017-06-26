#include "Window.h"
#include "shaders/gen/FontShader.h"
#include "shaders/gen/TextureShader.h"
#include "../../html/TagNode.h"
#include "../../html/TextNode.h"
#include <cmath>
#include <iostream>

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

    boxComponents.push_back(std::make_unique<BoxComponent>(0.0f, 1.0f, 1.0f, -64, windowWidth, windowHeight));
    boxComponents.push_back(std::make_unique<BoxComponent>(-512, 0.0f, 512, 512, windowWidth, windowHeight));

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

    window = glfwCreateWindow(640, 480, "NetRunner", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cout << "Could not create window" << std::endl;
        return false;
    }
    glfwSetWindowUserPointer(window, this);
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        thiz->windowWidth = width;
        thiz->windowHeight = height;
        for (const std::unique_ptr<BoxComponent> &boxComponent : thiz->boxComponents) {
            boxComponent->resize(width, height);
        }
        thiz->y = 950;
        for (const std::unique_ptr<Component> &component : thiz->components) {
            TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
            textComponent->resize(0, thiz->y, width, height);
            thiz->y -= textComponent->height;
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
}

const GLuint Window::compileShader(const GLenum shaderType, const char *shaderSource) const {
    const GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "Could not compile shader\n" << infoLog << std::endl;
    }

    return shader;
}

const GLuint Window::compileProgram(const GLuint vertexShader, const GLuint fragmentShader) const {
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "Could not compile program\n" << infoLog << std::endl;
    }

    return program;
}

void Window::render() {
    if (domDirty) {
        drawNode(domRootNode);
        domDirty = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(textureProgram);
    for (const std::unique_ptr<BoxComponent> &boxComponent : boxComponents) {
        boxComponent->render();
    }
    glUseProgram(fontProgram);
    for (const std::unique_ptr<Component> &component : components) {
        TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
        textComponent->render();
    }
    glfwPollEvents();
    glfwSwapBuffers(window);
}

void Window::setDOM(const std::shared_ptr<Node> rootNode) {
    domRootNode = rootNode;
    domDirty = true;
}

void Window::drawNode(const std::shared_ptr<Node> node) {
    if (node->nodeType == NodeType::TAG) {
        TagNode *tagNode = dynamic_cast<TagNode*>(node.get());
        std::unique_ptr<Component> component = tagNode->render(*tagNode, y, windowWidth, windowHeight);
        if (component) {
            components.push_back(tagNode->render(*tagNode, y, windowWidth, windowHeight));
            y -= component->height;
        }
    }
    for (std::shared_ptr<Node> child : node->children) {
        drawNode(child);
    }
}
