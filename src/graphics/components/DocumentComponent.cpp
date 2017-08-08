#include "DocumentComponent.h"
#include <cmath>
#include <iostream>
#include "../../Log.h"
#include "InputComponent.h"

DocumentComponent::DocumentComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int passedWindowWidth, const int passedWindowHeight) {
    //std::cout << "DocumentComponent::DocumentComponent" << std::endl;

    windowWidth = passedWindowWidth;
    windowHeight = passedWindowHeight;
    //std::cout << "DocumentComponent::DocumentComponent - window size: " << windowWidth << "x" << windowHeight << std::endl;
    
    x = rawX;
    y = rawY;
    width = rawWidth;
    height = rawHeight;
    
    if (height < 0) {
        std::cout << "DocumentComponent::DocumentComponent - height was less than zero" << std::endl;
        height = 0;
    }
    //std::cout << "DocumentComponent::DocumentComponent - our size" << static_cast<int>(width) << "x" << static_cast<int>(height) << std::endl;
    onMousemove=[this](int x, int y) {
        // set hover component
        static int lx = 0;
        static int ly = 0;
        //std::cout << "DocumentComponent::DocumentComponent:onMousemove - at " << x << "," << y << std::endl;
        if (lx == x && ly == y) {
            return;
        }
        lx = x;
        ly = y;
        //std::cout << "DocumentComponent::DocumentComponent:onMousemove - size " << this->windowWidth << "," << this->windowHeight << std::endl;
        this->hoverComponent = this->searchComponentTree(this->rootComponent, x, y);
        if (this->hoverComponent) {
            //std::cout << "DocumentComponent::DocumentComponent:onMousemove - hovering over " << typeOfComponent(this->hoverComponent) << " component" << std::endl;
            if (this->hoverComponent->onMousemove) {
                // this could communicate the cursor to use
                this->hoverComponent->onMousemove(x, y);
            }
            if (this->hoverComponent->onClick) {
                glfwSetCursor(this->win->window, this->win->cursorHand);
            } else {
                glfwSetCursor(this->win->window, this->win->cursorIbeam);
            }
        } else {
            glfwSetCursor(this->win->window, this->win->cursorArrow);
        }
    };
    onWheel=[this](int x, int y) {
        //std::cout << "scroll yDelta: " << y << std::endl;
        this->scrollY -= y;
        if (this->scrollY < 0) {
            this->scrollY = 0;
        } else if (this->scrollY > this->scrollHeight) {
            this->scrollY = this->scrollHeight;
        }
        //std::cout << "scroll pos: " << scrollY << "/" << scrollHeight << std::endl;
        //std::cout << "y: " << static_cast<int>(this->y) << " - " << this->scrollY << std::endl;
        //std::cout << "root.y: " << static_cast<int>(rootComponent->y) << std::endl;
        //std::cout << "windowSize: " << windowWidth << "," << windowHeight << std::endl;
        
        // ajdust root position
        rootComponent->y = this->y + this->scrollY;
        //std::cout << "now root.y: " << static_cast<int>(rootComponent->y) << std::endl;
        
        // reset root size
        rootComponent->windowWidth = windowWidth;
        rootComponent->windowHeight = windowHeight;
        
        //printComponentTree(rootComponent, 0);
        
        // this takes so long, we may want to delay until the input is adjusted
        rootComponent->layout(); // need to update the vertices
        //rootComponent->y = this->y - this->scrollY;
        //std::cout << "after root.y: " << static_cast<int>(rootComponent->y) << std::endl;
        
        // don't need this
        //this->renderDirty = true;
    };
    onMousedown=[this](int x, int y) {
        std::cout << "document left press" << std::endl;
        if (this->hoverComponent) {
            if (this->focusedComponent != this->hoverComponent) {
                // blur old component
                if (this->focusedComponent) {
                    if (this->focusedComponent->onBlur) {
                        this->focusedComponent->onBlur();
                    }
                }
                // focus new component
                if (this->hoverComponent->onFocus) {
                    this->hoverComponent->onFocus();
                }
            }
            this->focusedComponent = this->hoverComponent;
            if (this->focusedComponent->onMousedown) {
                //std::cout << "click event" << std::endl;
                this->focusedComponent->onMousedown(x, y);
            }
        }
    };
    onMouseup=[this](int x, int y) {
        std::cout << "document left release" << std::endl;
        if (this->hoverComponent) {
            //std::cout << "DocumentComponent::DocumentComponent:onMouseup - hovering over " << typeOfComponent(this->hoverComponent) << " component" << std::endl;
            if (this->focusedComponent != this->hoverComponent) {
                // blur old component
                if (this->focusedComponent) {
                    if (this->focusedComponent->onBlur) {
                        this->focusedComponent->onBlur();
                    }
                }
                // focus new component
                if (this->hoverComponent->onFocus) {
                    this->hoverComponent->onFocus();
                }
            }
            this->focusedComponent = this->hoverComponent;
            if (this->focusedComponent->onMouseup) {
                //std::cout << "click event" << std::endl;
                this->focusedComponent->onMouseup(x, y);
            }
            if (this->focusedComponent->onClick) {
                //std::cout << "click event" << std::endl;
                this->focusedComponent->onClick();
            }
        }
    };
    onKeyup=[this](int key, int scancode, int action, int mods) {
        //std::cout << "DocumentComponent::DocumentComponent:onKeyup" << std::endl;
        InputComponent *inputComponent = dynamic_cast<InputComponent*>(this->focusedComponent.get());
        if (inputComponent) {
            //std::cout << "inputComponent is focused, key pressed " << key << " action: " <<action << std::endl;
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
        }
    };
}

void DocumentComponent::printComponentTree(const std::shared_ptr<Component> &component, int depth) {
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

void DocumentComponent::render() {
    //std::cout << "DocumentComponent::render" << std::endl;
    if (domDirty) {
        const std::clock_t begin = clock();
        createComponentTree(domRootNode, rootComponent);
        const std::clock_t end = clock();
        std::cout << "built & laid out document components in: " << std::fixed << ((static_cast<double>(end - begin)) / CLOCKS_PER_SEC) << std::scientific << " seconds" << std::endl;
        //printComponentTree(rootComponent, 0);
        domDirty = false;
        renderDirty = true;
        //std::cout << "root Height: " << static_cast<int>(rootComponent->height) << " window Height: " << windowHeight << " y " << static_cast<int>(this->y) << std::endl;
        scrollHeight = std::max(0, static_cast<int>(rootComponent->height - (windowHeight + (this->y * 2))));
    }
    if (renderDirty) {
        glUseProgram(win->textureProgram);
        renderBoxComponents(rootComponent);
        glUseProgram(win->fontProgram);
        renderComponents(rootComponent);
    }
}

// create this component and all it's children
void DocumentComponent::createComponentTree(const std::shared_ptr<Node> node, const std::shared_ptr<Component> &parentComponent) {
    std::shared_ptr<Component> component = componentBuilder.build(node, parentComponent, windowWidth, windowHeight);
    
    if (!component) {
        //std::cout << "DocumentComponent::createComponentTree - no component" << std::endl;
        return;
    }
    if (node==domRootNode) {
        // if this is the root node
        component->reqWidth = windowWidth;
        component->reqHeight = windowHeight;
    }
    // create children elements
    for (std::shared_ptr<Node> child : node->children) {
        createComponentTree(child, component);
    }
}

// draw this component and all it's children
void DocumentComponent::renderComponents(std::shared_ptr<Component> component) {
    if (!component) {
        std::cout << "DocumentComponent::renderComponents - got null passed" << std::endl;
        return;
    }
    TextComponent *textComponent = dynamic_cast<TextComponent*>(component.get());
    if (textComponent) {
        textComponent->render();
    }
    // is this needed?
    if (component->children.empty()) {
        return;
    }
    for (std::shared_ptr<Component> &child : component->children) {
        renderComponents(child);
    }
}

void DocumentComponent::renderBoxComponents(std::shared_ptr<Component> component) {
    if (!component) {
        std::cout << "DocumentComponent::renderBoxComponents - got null passed" << std::endl;
        return;
    }
    // render non-text components too
    BoxComponent *boxComponent = dynamic_cast<BoxComponent*>(component.get());
    //inputComponent->render();
    if (boxComponent) {
        /*
         GLenum glErr=glGetError();
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

// used for picking
std::shared_ptr<Component> DocumentComponent::searchComponentTree(const std::shared_ptr<Component> &component, const int x, const int y) {
    if (component->children.empty()) {
        //std::cout << "DocumentComponent::searchComponentTree - component at " << static_cast<int>(component->x) << "," << static_cast<int>(component->y) << " size " << static_cast<int>(component->width) << "," << static_cast<int>(component->height) << std::endl;
        //std::cout << "DocumentComponent::searchComponentTree - y search: " << static_cast<int>(-component->y) << "<" << static_cast<int>(y) << "<" << static_cast<int>(-component->y + component->height) << std::endl;
        if (-component->y < y && -component->y + component->height > y) {
            //std::cout << "DocumentComponent::searchComponentTree - x search: " << static_cast<int>(component->x) << "<" << static_cast<int>(x) << "<" << static_cast<int>(component->x + component->width) << std::endl;
            if (component->x < x && component->x + component->width > x) {
                //std::cout << "hit " << typeOfComponent(component) << std::endl;
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
void DocumentComponent::navTo(const std::string url) {
    logDebug() << "navTo(" << url << ")" << std::endl;
    currentURL = currentURL.merge(URL(url));
    logDebug() << "go to: " << currentURL << std::endl;
    setWindowContent(currentURL);
}
