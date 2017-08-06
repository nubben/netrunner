#include "../../URL.h"
#include "Window.h"
#include "shaders/gen/FontShader.h"
#include "shaders/gen/TextureShader.h"
#include "../../html/TagNode.h"
#include "../../html/TextNode.h"

#include "../../networking/HTTPRequest.h"
#include "../../html/HTMLParser.h"
#include "../../StringUtils.h"
#include "../../URL.h"
#include "../../Log.h"

#include <cmath>
#include <ctime>
#include <iostream>

void deleteComponent(std::shared_ptr<Component> &component);
void deleteNode(std::shared_ptr<Node> node);

Window::~Window() {
    if (fontProgram) {
        glDeleteProgram(fontProgram);
    }
    if (textureProgram) {
        glDeleteProgram(textureProgram);
    }

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
    int glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "window::init - post initGL - not ok: " << glErr << std::endl;
    }

    //UI
    // at 0x(640-64) size (1024w, 64h)
    boxComponents.push_back(std::make_unique<BoxComponent>(0.0f, 640.0f - 64.0f, 1024.0f, 64.0f, windowWidth, windowHeight));

    //boxComponents.push_back(std::make_unique<BoxComponent>(0.0f, -96, 125, 13, windowWidth, windowHeight));
    // grows from center
    //boxComponents.push_back(std::make_unique<AnimeComponent>(0.0f, -80, 16, 16, windowWidth, windowHeight));
    //boxComponents.push_back(std::make_unique<InputComponent>(0.0f, -56.0f, 125.0f, 13.0f, windowWidth, windowHeight));
    
    // draws 13 px up from the bottom of 0
    //boxComponents.push_back(std::make_unique<InputComponent>(0.0f, 0.0f, 12.0f, 13.0f, windowWidth, windowHeight));
    // this works fine, we can translate and resize
    //boxComponents.back()->y = -56;
    //boxComponents.back()->width = 125;
    //boxComponents.back()->resize(windowWidth, windowHeight);
    //boxComponents.push_back(std::make_unique<InputComponent>(0.0f, 20.0f, 123.0f, 13.0f, windowWidth, windowHeight));
    
    //Mascot
    boxComponents.push_back(std::make_unique<AnimeComponent>(768.0f, 0.0f, 512.0f, 512.0f, windowWidth, windowHeight));
    
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
    //glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    glfwSetErrorCallback([](int error, const char* description) {
        std::cout << "glfw error [" << error << "]:" << description << std::endl;
    });

    window = glfwCreateWindow(1024, 640, "NetRunner", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cout << "Could not create window" << std::endl;
        return false;
    }
    // replace first parameter of all these callbacks with our window object instead of a GLFWwindow
    glfwSetWindowUserPointer(window, this);
    // set window w/h
    //glfwGetFramebufferSize(window, &windowWidth, &windowHeight); // in pixels
    glfwGetWindowSize(window, &windowWidth, &windowHeight); // use screen coordinates (not pixels) more retina friendly
    
    // set up event callbacks
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *win, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glfwSetWindowSizeCallback(window, [](GLFWwindow *win, int width, int height) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        //
        thiz->windowWidth = width;
        thiz->windowHeight = height;
        thiz->delayResize = 1;
    });
    cursorHand = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    cursorArrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    cursorIbeam = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    glfwSetCursorPosCallback(window, [](GLFWwindow *win, double xPos, double yPos) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        thiz->cursorX = xPos;
        thiz->cursorY = yPos;
        thiz->hoverComponent = thiz->searchComponentTree(thiz->rootComponent, thiz->cursorX, (thiz->windowHeight - thiz->cursorY) + ((-thiz->transformMatrix[13] / 2) * thiz->windowHeight));
        if (thiz->hoverComponent) {
            //std::cout << "hover event" << std::endl;
            if (thiz->hoverComponent->onClick) {
                glfwSetCursor(thiz->window, thiz->cursorHand);
            } else {
                glfwSetCursor(thiz->window, thiz->cursorIbeam);
            }
        } else {
            glfwSetCursor(thiz->window, thiz->cursorArrow);
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow *win, double xOffset, double yOffset) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        // yOffset is a delta vector
        thiz->transformMatrix[13] += -yOffset * 0.1;
        
        // 2.0 is one screen height
        // we draw from 0 downwards (y+), so can't scroll past our starting draw point
        if (thiz->transformMatrix[13]<2) {
            thiz->transformMatrix[13]=2;
        }
        // calculate scroll max by calculating how many screens are in the rootComponent's Height
        if (thiz->transformMatrix[13]>std::max((thiz->rootComponent->height)/(thiz->windowHeight)*2.0f, 2.0f)) {
            thiz->transformMatrix[13]=std::max((thiz->rootComponent->height)/(thiz->windowHeight)*2.0f, 2.0f);
        }
        //std::cout << "scroll y is at " << thiz->transformMatrix[13] << "/" << static_cast<int>((thiz->transformMatrix[13]*10000) << std::endl;
        thiz->transformMatrixDirty = true;
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *win, int button, int action, int mods) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            std::cout << "left press" << std::endl;
            if (thiz->hoverComponent) {
                //std::cout << "focus event" << std::endl;
                if (thiz->focusedComponent != thiz->hoverComponent) {
                    // blur old component
                    if (thiz->focusedComponent) {
                        if (thiz->focusedComponent->onBlur) {
                            thiz->focusedComponent->onBlur();
                        }
                    }
                    // focus new component
                    if (thiz->hoverComponent->onFocus) {
                        thiz->hoverComponent->onFocus();
                    }
                }
                thiz->focusedComponent = thiz->hoverComponent;
                if (thiz->focusedComponent->onMousedown) {
                    //std::cout << "click event" << std::endl;
                    thiz->focusedComponent->onMousedown(thiz->cursorX, thiz->cursorY);
                }
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            std::cout << "left release" << std::endl;
            if (thiz->hoverComponent) {
                //std::cout << "focus event" << std::endl;
                if (thiz->focusedComponent != thiz->hoverComponent) {
                    // blur old component
                    if (thiz->focusedComponent) {
                        if (thiz->focusedComponent->onBlur) {
                            thiz->focusedComponent->onBlur();
                        }
                    }
                    // focus new component
                    if (thiz->hoverComponent->onFocus) {
                        thiz->hoverComponent->onFocus();
                    }
                }
                thiz->focusedComponent = thiz->hoverComponent;
                if (thiz->focusedComponent->onMouseup) {
                    //std::cout << "click event" << std::endl;
                    thiz->focusedComponent->onMouseup(thiz->cursorX, thiz->cursorY);
                }
                if (thiz->focusedComponent->onClick) {
                    //std::cout << "click event" << std::endl;
                    thiz->focusedComponent->onClick();
                }
            }
        }
    });
    
    // works with utf-32 but we'll lkeep the low level for now
    /*
    glfwSetCharCallback(window, [](GLFWwindow* win, unsigned int codepoint) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        std::cout << "Window::glfwSetCharCallback - codepoint: " << codepoint << std::endl;
    });
    */
    
    glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        Window *thiz = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
        // we're focused on something
        if (thiz->focusedComponent) {
            InputComponent *inputComponent = dynamic_cast<InputComponent*>(thiz->focusedComponent.get());
            if (inputComponent) {
                //std::cout << "inputComponent is focsued, key pressed " << key << " action: " <<action << std::endl;
                // action 1 is down, 0 is up, 2 is a repeat
                if (action == 0 || action == 2) {
                    // key up
                    // it's always uppercase...
                    if (key == 259) {
                        inputComponent->backSpace();
                    } else if (key == 257) {
                        std::cout << "enter!" << std::endl;
                    } else {
                        if (key < 256) {
                            if (mods & GLFW_MOD_SHIFT) {
                                // SHIFT
                                if (key == GLFW_KEY_SLASH) key='?';
                                if (key == GLFW_KEY_APOSTROPHE) key='"';
                                if (key == GLFW_KEY_COMMA) key='<';
                                if (key == GLFW_KEY_MINUS) key='_';
                                if (key == GLFW_KEY_PERIOD) key='>';
                                if (key == GLFW_KEY_SEMICOLON) key=':';
                                if (key == GLFW_KEY_EQUAL) key='+';
                                if (key == GLFW_KEY_LEFT_BRACKET) key='{';
                                if (key == GLFW_KEY_BACKSLASH) key='|';
                                if (key == GLFW_KEY_RIGHT_BRACKET) key='}';
                                if (key == GLFW_KEY_GRAVE_ACCENT) key='~';
                                
                            } else {
                                // no shift or caplocks
                                // basically: when SHIFT isn't pressed but key is in A-Z range, add ascii offset to make it lower case
                                if (key >= 'A' && key <= 'Z') {
                                    key += 'a' - 'A';
                                }
                            }
                            inputComponent->addChar(key);
                        } // otherwise I think it's some weird control char
                    }
                }
                return;
            }
        }
        switch (key) {
            case GLFW_KEY_1:
            case GLFW_KEY_2:
            case GLFW_KEY_3:
            case GLFW_KEY_4:
            case GLFW_KEY_5:
            case GLFW_KEY_6:
            case GLFW_KEY_7:
            case GLFW_KEY_8:
            case GLFW_KEY_9:
            case GLFW_KEY_0:
                std::cout << "Key was pressed: " << key << std::endl;
                //thiz->transformMatrixIndex = key - GLFW_KEY_0;
                //std::cout << thiz->transformMatrixIndex << std::endl;
                break;
            default:
                break;
        }
        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            std::cout << "Q was pressed. Exiting." << std::endl;
            exit(0);
        }
        if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
            printf("Printing NodeTree\n\n");
            printNode(thiz->domRootNode, 1);
            printf("\n\n");
        }
        if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
            printf("Printing ComponentTree\n\n");
            thiz->printComponentTree(thiz->rootComponent, 0);
            printf("\n\n");
        }

        int yOffsetScroll = 1;
        if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            std::cout << "J is/was pressed. Scrolling down." << std::endl;
            thiz->transformMatrix[13] += yOffsetScroll * 0.1;
            thiz->transformMatrixDirty = true;
        }
        if (key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            std::cout << "K is/was pressed. Scrolling up." << std::endl;
            thiz->transformMatrix[13] += -yOffsetScroll * 0.1;
            thiz->transformMatrixDirty = true;
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

    int glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "window::initGL - blend, clear, texParam - not ok: " << glErr << std::endl;
    }
    
    const GLuint fontVertexShader = compileShader(GL_VERTEX_SHADER, fontVertexShaderSource);
    const GLuint fontFragmentShader = compileShader(GL_FRAGMENT_SHADER, fontFragmentShaderSource);
    fontProgram = compileProgram(fontVertexShader, fontFragmentShader);
    glDeleteShader(fontVertexShader);
    glDeleteShader(fontFragmentShader);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "window::initGL - font program - not ok: " << glErr << std::endl;
    }

    const GLuint textureVertexShader = compileShader(GL_VERTEX_SHADER, textureVertexShaderSource);
    const GLuint textureFragmentShader = compileShader(GL_FRAGMENT_SHADER, textureFragmentShaderSource);
    textureProgram = compileProgram(textureVertexShader, textureFragmentShader);
    glDeleteShader(textureVertexShader);
    glDeleteShader(textureFragmentShader);
    glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "window::initGL - texture program - not ok: " << glErr << std::endl;
    }

    //std::cout << "OpenGL is set up" << std::endl;

    return true;
}

GLuint Window::compileShader(const GLenum shaderType, const char *shaderSource) const {
    const GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    int glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "window::compileShader - compileShader - not ok: " << glErr << std::endl;
    }
    
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

    int glErr=glGetError();
    if(glErr != GL_NO_ERROR) {
        std::cout << "window::compileProgram - glLinkProgram - not ok: " << glErr << std::endl;
    }
    
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "Could not compile program\n" << infoLog << std::endl;
    }

    return program;
}

void Window::render() {
    if (delayResize) {
        delayResize--;
        if (delayResize) {
            return;
        }
        std::cout << "restarting drawing" << std::endl;
        
        // just delete and recreate
        boxComponents.clear();
        //UI
        // at 0x(640-64) size (1024w, 64h)
        boxComponents.push_back(std::make_unique<BoxComponent>(0.0f, windowHeight - 64.0f, windowWidth, 64.0f, windowWidth, windowHeight));
        
        // draws 13 px up from the bottom of 0
        //boxComponents.push_back(std::make_unique<InputComponent>(0.0f, 0.0f, 123.0f, 13.0f, windowWidth, windowHeight));
        
        //Mascot
        boxComponents.push_back(std::make_unique<AnimeComponent>(windowWidth * 0.75f, 0.0f, 512.0f, 512.0f, windowWidth, windowHeight));
        
        /*
        for (const std::unique_ptr<BoxComponent> &boxComponent : boxComponents) {
            //boxComponent->windowWidth = windowWidth;
            //boxComponent->windowHeight = windowHeight;
            boxComponent->resize(windowWidth, windowHeight);
        }
         */
        //std::cout << "resizing" << std::endl;
        const std::clock_t begin = clock();
        //resizeComponentTree(rootComponent, windowWidth, windowHeight);
        rootComponent->windowWidth = windowWidth;
        rootComponent->windowHeight = windowHeight;
        rootComponent->layout();
        const std::clock_t end = clock();
        std::cout << "resized components in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;

        // recalculate scroll max by calculating how many screens are in the rootComponent's Height
        if (transformMatrix[13]>std::max((rootComponent->height)/(windowHeight)*2.0f, 2.0f)) {
            transformMatrix[13]=std::max((rootComponent->height)/(windowHeight)*2.0f, 2.0f);
            transformMatrixDirty = true;
        }
        
        //thiz->printComponentTree(thiz->rootComponent, 0);
        //printComponentTree(rootComponent, 0);
        renderDirty = true;
    }
    if (domDirty) {
        const std::clock_t begin = clock();
        createComponentTree(domRootNode, rootComponent);
        const std::clock_t end = clock();
        std::cout << "built & laid out components in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
        //printComponentTree(rootComponent, 0);
        domDirty = false;
        renderDirty = true;
    }
    if (renderDirty || transformMatrixDirty) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "window::render - box render start - not ok: " << glErr << std::endl;
        }
        
        glUseProgram(textureProgram);
        glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "window::render - glUseProgram - not ok: " << glErr << std::endl;
        }
        //renderBoxComponents(rootComponent);
        for (const std::unique_ptr<BoxComponent> &boxComponent : boxComponents) {
            boxComponent->render();
        }
        renderBoxComponents(rootComponent);
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
        glfwSwapBuffers(window);
        
        // update 2nd buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(textureProgram);
        //renderBoxComponents(rootComponent);
        for (const std::unique_ptr<BoxComponent> &boxComponent : boxComponents) {
            boxComponent->render();
        }
        renderBoxComponents(rootComponent);
        // it's quick but done on scroll
        glUseProgram(fontProgram);
        renderComponents(rootComponent);
        glfwSwapBuffers(window);
        
        renderDirty = false;
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void deleteComponent(std::shared_ptr<Component> &component) {
    // delete all my child first
    for (std::shared_ptr<Component> child : component->children) {
        deleteComponent(child);
    }
    component->parent=nullptr;
    component->children.clear();
    // now delete self
}

void deleteNode(std::shared_ptr<Node> node) {
    for (std::shared_ptr<Node> child : node->children) {
        deleteNode(child);
    }
    node->parent=nullptr;
    node->children.clear();
    node->component=nullptr; // disassociate component
}

void Window::setDOM(const std::shared_ptr<Node> rootNode) {
    // reset rootComponent
    if (rootComponent) {
        deleteComponent(rootComponent);
    }
    if (domRootNode) {
        deleteNode(domRootNode);
    }

    // reset scroll position
    transformMatrix[13] = 2;
    transformMatrixDirty = true;

    // new root component
    rootComponent = std::make_shared<Component>();
    rootComponent->y = 0;
    domRootNode = rootNode;
    domDirty = true;
}

void Window::createComponentTree(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent) {
    std::shared_ptr<Component> component = componentBuilder.build(node, parentComponent, windowWidth, windowHeight);
    
    // ComponentBuilder now calls setParent and setParents adds children
    //component->setParent(parentComponent);
    // set parent of the newly created component
    //component->parent = parentComponent;
    //if (parentComponent) {
        // add new component as child to parent
        //parentComponent->children.push_back(component);
    //}
    
    if (node==domRootNode) {
        // if this is the root node
        component->reqWidth = windowWidth;
        component->reqHeight = windowHeight;
    }
    
    // create children elements
    for (std::shared_ptr<Node> child : node->children) {
        createComponentTree(child, component);
    }
    // update parents, creation brings them up to date
    //updateComponentSize(component);
}

void Window::printComponentTree(const std::shared_ptr<Component> &component, int depth) {
    for (int i = 0; i < depth; i++) {
        std::cout << '\t';
    }
    TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
    if (textComponent) {
        std::cout << std::fixed << "X: " << static_cast<int>(textComponent->x) << " Y: " << static_cast<int>(textComponent->y) << " WIDTH: " << static_cast<int>(textComponent->width) << " HEIGHT: " << static_cast<int>(textComponent->height) << " INLINE: " << textComponent->isInline << " TEXT: " << textComponent->text << std::endl;
    }
    else {
        std::cout << std::fixed << "X: " << static_cast<int>(component->x) << " Y: " << static_cast<int>(component->y) << " WIDTH: " << static_cast<int>(component->width) << " HEIGHT: " << static_cast<int>(component->height) << " INLINE: " << component->isInline << std::endl;
    }
    for (std::shared_ptr<Component> child : component->children) {
        printComponentTree(child, depth + 1);
    }
}

void Window::renderBoxComponents(std::shared_ptr<Component> component) {
    if (!component) {
        std::cout << "Window::renderBoxComponents - got null passed" << std::endl;
        return;
    }
    // render non-text components too
    BoxComponent *boxComponent = dynamic_cast<BoxComponent*>(component.get());
    //inputComponent->render();
    if (boxComponent) {
        /*
        int glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "Window::renderBoxComponents - box render start - not ok: " << glErr << std::endl;
        }
        */
        /*
        glUseProgram(textureProgram);
        glErr=glGetError();
        if(glErr != GL_NO_ERROR) {
            std::cout << "glUseProgram - not ok: " << glErr << std::endl;
        }
        */
        //std::cout << "Window::renderBoxComponents - Rendering at " << (int)boxComponent->x << "x" << (int)boxComponent->y << std::endl;
        boxComponent->render();
    }
    /*
     InputComponent *inputComponent = dynamic_cast<InputComponent*>(component.get());
     if (inputComponent) {
     glUseProgram(textureProgram);
     inputComponent->render();
     }
     */
    // is this needed?
    if (component->children.empty()) {
        return;
    }
    for (std::shared_ptr<Component> &child : component->children) {
        renderBoxComponents(child);
    }
}

void Window::renderComponents(std::shared_ptr<Component> component) {
    if (!component) {
        std::cout << "Window::renderComponents - got null passed" << std::endl;
        return;
    }
    TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
    if (textComponent) {
        //glUseProgram(fontProgram);
        textComponent->render();
    } else {
        // render non-text components too
        /*
        BoxComponent *boxComponent = dynamic_cast<BoxComponent*>(component.get());
        //inputComponent->render();
        if (boxComponent) {
            int glErr=glGetError();
            if(glErr != GL_NO_ERROR) {
                std::cout << "box render start - not ok: " << glErr << std::endl;
            }
            glUseProgram(textureProgram);
            glErr=glGetError();
            if(glErr != GL_NO_ERROR) {
                std::cout << "glUseProgram - not ok: " << glErr << std::endl;
            }
            boxComponent->render();
        }
        */
        /*
        InputComponent *inputComponent = dynamic_cast<InputComponent*>(component.get());
        if (inputComponent) {
            glUseProgram(textureProgram);
            inputComponent->render();
        }
        */
    }
    // is this needed?
    if (component->children.empty()) {
        return;
    }
    for (std::shared_ptr<Component> &child : component->children) {
        renderComponents(child);
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

// moving naviagtion closer to window, as window is now the owner of currentURL
// preparation for multiple HTML documents
void Window::navTo(std::string url) {
    logDebug() << "navTo(" << url << ")" << std::endl;
    currentURL = currentURL.merge(URL(url));
    logDebug() << "go to: " << currentURL << std::endl;
    setWindowContent(currentURL);
}

/*
 void handleRequest(const HTTPResponse &response) {
 std::cout << "main:::handleRequest - statusCode: " << response.statusCode << std::endl;
 if (response.statusCode == 200) {
 const std::unique_ptr<HTMLParser> parser = std::make_unique<HTMLParser>();
 const std::clock_t begin = clock();
 std::shared_ptr<Node> rootNode = parser->parse(response.body);
 const std::clock_t end = clock();
 std::cout << "Parsed document in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
 window->setDOM(rootNode);
 }
 else if (response.statusCode == 301) {
 std::string location;
 if (response.properties.find("Location")==response.properties.end()) {
 if (response.properties.find("location")==response.properties.end()) {
 std::cout << "::handleRequest - got 301 without a location" << std::endl;
 for(auto const &row : response.properties) {
 std::cout << "::handleRequest - " << row.first << "=" << response.properties.at(row.first) << std::endl;
 }
 return;
 } else {
 location = response.properties.at("location");
 }
 } else {
 location = response.properties.at("Location");
 }
 std::cout << "Redirect To: " << location << std::endl;
 std::shared_ptr<URI> uri = parseUri(location);
 const std::unique_ptr<HTTPRequest> request = std::make_unique<HTTPRequest>(uri);
 request->sendRequest(handleRequest);
 return;
 }
 else {
 std::cout << "Unknown Status Code: " << response.statusCode << std::endl;
 }
 }
 */

// tried to make a window method
void handleRequest(const HTTPResponse &response) {
    std::cout << "main:::handleRequest - statusCode: " << response.statusCode << std::endl;
    if (response.statusCode == 200) {
        const std::unique_ptr<HTMLParser> parser = std::make_unique<HTMLParser>();
        const std::clock_t begin = clock();
        std::shared_ptr<Node> rootNode = parser->parse(response.body);
        const std::clock_t end = clock();
        std::cout << "Parsed document in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
        window->setDOM(rootNode);
    }
    else if (response.statusCode == 301) {
        std::string location;
        if (response.properties.find("Location")==response.properties.end()) {
            if (response.properties.find("location")==response.properties.end()) {
                std::cout << "::handleRequest - got 301 without a location" << std::endl;
                for(auto const &row : response.properties) {
                    std::cout << "::handleRequest - " << row.first << "=" << response.properties.at(row.first) << std::endl;
                }
                return;
            } else {
                location = response.properties.at("location");
            }
        } else {
            location = response.properties.at("Location");
        }
        std::cout << "Redirect To: " << location << std::endl;
        std::tuple<std::unique_ptr<URL>,enum URIParseError> result = parseUri(location);
        if (std::get<1>(result) != URI_PARSE_ERROR_NONE) {
            // TODO We probably wanna handle this better..
            std::cerr << "error parsing uri" << std::endl;
            return;
        }
        std::unique_ptr<URL> uri = std::move(std::get<0>(result));
        const std::unique_ptr<HTTPRequest> request = std::make_unique<HTTPRequest>(std::move(uri));
        request->sendRequest(handleRequest);
        return;
    }
    else {
        std::cout << "Unknown Status Code: " << response.statusCode << std::endl;
    }
}
