#include "Window.h"
#include "shaders/gen/FontShader.h"
#include "shaders/gen/TextureShader.h"
#include "../../html/TagNode.h"
#include "../../html/TextNode.h"
#include <cmath>
#include <ctime>
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
        //printComponentTree(rootComponent, 0);
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

void Window::createComponentTree(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent) {
    std::shared_ptr<Component> component = componentBuilder.build(node, parentComponent, windowWidth, windowHeight);

    // set parent of the newly created component
    component->parent = parentComponent;
    if (parentComponent) {
        // add new component as child to parent
        parentComponent->children.push_back(component);
    }

    // adjust width of parents if we're not a block component
    if (component->isInline) {
        for (std::shared_ptr<Component> parent = parentComponent; parent != nullptr; parent = parent->parent) {
            parent->width += component->width; // update parents width with our child width
        }
    }

    // create children elements
    int maxHeight=0;
    for (std::shared_ptr<Node> child : node->children) {
        createComponentTree(child, component);
        if (child->component) {
            maxHeight=std::max(maxHeight, static_cast<int>(child->component->height));
            // update width
            /*
            for (std::shared_ptr<Component> parent = parentComponent; parent != nullptr; parent = parent->parent) {
                parent->width+=child->component->width;
            }
            */
        }
    }
    // update height
    for (std::shared_ptr<Component> parent = parentComponent; parent != nullptr; parent = parent->parent) {
        parent->height+=maxHeight;
    }
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
        // + parent->width is wrong here
        // for all parent's children
        int maxParentWidth = 0; // float?
        for (std::shared_ptr<Component> child : component->parent->children) {
            // find max width
            maxParentWidth = std::max(maxParentWidth, static_cast<int>(child->width));
        }
        int found=0;
        std::shared_ptr<Component> prev=0;
        for (std::shared_ptr<Component> child : component->parent->children) {
            if (child==component) {
                found=1;
                break;
            }
            prev=child;
        }
        int x=component->parent->x;
        int y=component->parent->y;
        if (found && prev) {
            // 2nd or last
            if (prev->isInline) {
                if (component->isInline) {
                    x=prev->x+prev->width;
                    y=prev->y; // keep on same line
                }
            } else {
                // last was block
                y=prev->y-prev->height;
            }
        } else {
            // first, there will be no width to add
        }
        textComponent->resize(x, y, width, height);

        // update parent sizes
        // does anything reset them?
        int heightIncrease = (component->parent->y - component->parent->height) - (component->y - component->height);
        // look at old width vs new max width
        int widthIncrease = maxParentWidth - component->parent->width;
        //std::cout << "widthIncrease: " << widthIncrease << std::endl;
        for (std::shared_ptr<Component> parent = component->parent; parent != nullptr; parent = parent->parent) {
            parent->width += widthIncrease;
            parent->height += heightIncrease;
        }
    }
    else {
        //std::cout << "inlined: " << component->isInline << std::endl;
        // if we have a parent, change our position based on parent's position
        if (component->parent) {
            component->x = component->parent->x;
            component->y = component->parent->y - component->parent->height;
            std::shared_ptr<Component> last=0;
            int found=0;
            for (std::shared_ptr<Component> child : component->parent->children) {
                if (child==component) {
                    found=1;
                    break;
                }
                last=child;
            }
            if (found && last) {
                //std::cout << "previous inLine was " << last->isInline << " at " << static_cast<int>(last->x) << "," << static_cast<int>(last->y)<< std::endl;
                if (last->isInline) {
                    if (component->isInline) {
                      //std::cout << "we're inline put at: " << static_cast<int>(last->x) << "+" << static_cast<int>(last->width) << std::endl;
                      component->x += last->x+static_cast<int>(last->width);
                      // copy last known inline starting y position
                      // tho if there's a wrap we should start from there
                      // so if x is 3 lines, we need to start at line #2
                      // well we don't know what a line is and height can vary between components
                      // maybe we shouldn't wrap in the rasterizer but here
                      // you'd give the rasterizer a width and the text
                      // and it returns a component per line
                      // or a text component needs to set an ending y point
                      //
                      // two inline components, first is 3 lines tall, where does the 2nd's y position start
                        // at the 2nd line but how do you calculate that if each inline component can have different sizes
                        // (line-height/font-size)
                      component->y = last->y;
                    } else {
                      // span to div
                      component->y = last->y-last->height;
                    }
                } else {
                    if (component->isInline) {
                        //std::cout << "we're inline but previous wasnt, forcing parent x" << std::endl;
                        component->x = component->parent->x;
                    } else {

                    }
                }
            } else {
              // no prev
              // if last is null, we're likely the first child and should be at parent->x
              // if not found, we're p. fucked eh?
            }
        }
        // if we're inline, increase our position by parent's w/h
        // parent has a block element, bumps the width to 698
        // we're inline
        // we need to start at 0 because our previous component wasn't inLine
        // this doesn't make any sense
        if (component->isInline) {
            // start at the top of parent
            //component->y += component->parent->height;
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
