#include "Component.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "TextComponent.h"

Component::~Component() {
}


void Component::resize(const int passedWindowWidth, const int passedWindowHeight) {
    //std::cout << "Component::resize" << std::endl;
    windowWidth = passedWindowWidth;
    windowHeight = passedWindowHeight;
    /*
    if (boundToPage) {
        
    } else {
     */
        // no vertices in component
        /*
        float vx = x;
        float vy = y;
        float vWidth = width;
        float vHeight = height;
        pointToViewport(vx, vy);
        distanceToViewport(vWidth, vHeight);
        
        vertices[(0 * 5) + 0] = vx;
        vertices[(0 * 5) + 1] = vy + vHeight;
        vertices[(1 * 5) + 0] = vx + vWidth;
        vertices[(1 * 5) + 1] = vy + vHeight;
        vertices[(2 * 5) + 0] = vx + vWidth;
        vertices[(2 * 5) + 1] = vy;
        vertices[(3 * 5) + 0] = vx;
        vertices[(3 * 5) + 1] = vy;
        
        verticesDirty = true;
        */
    //}
}

void Component::setParent(std::shared_ptr<Component> passedParent) {
    if (!passedParent) {
        std::cout << "Component::setParent is empty" << std::endl;
    }
    if (parent) {
        std::cout << "Component::setParent - triggering component move" << std::endl;
    }
    // set parent of the newly created component
    parent = passedParent;
    
    // update our position
    x = passedParent->x;
    y = passedParent->y - passedParent->height;
    //std::cout << "Component::setParent - placing at " << static_cast<int>(x) << "," << static_cast<int>(y) << std::endl;
    
    // add new component as child to parent
    //std::shared_ptr<Component> child=*new std::shared_ptr<Component>(this);
    //parent->children.push_back(child);
}

// window size is required because we need to know if our x is placed outside bounds
// our size is not required
// but all previous children sizes and positions are
void Component::layout() {
    // back up current position
    //int lx = x;
    //int ly = y;
    
    // if we want to position the root component and layout (because to move things we need to trigger a resize)
    // then we need this to not reset because it will affect the position we're trying to relayout them out at
    // reset position
    //x = 0;
    //y = 0;

    /*
    TextComponent *textComponent = dynamic_cast<TextComponent*>(this);
    if (textComponent) {
        std::cout << "Component::layout[" << textComponent->text << "]" << std::endl;
    }
    */
    
    // if we're a child, get our parents position
    if (parent) {
        //std::cout << "Component::layout - copying position from parent: " << (int)parent->x << "x" << (int)parent->y << std::endl;

        x = parent->x;
        y = parent->y;
        // if we have sibilings see if they're inline or block
        if (parent->children.size()) {
            //std::cout << "Component::layout - parent children: " << parent->children.size() << std::endl;
            if (previous) {
                /*
                TextComponent *prevTextComponent = dynamic_cast<TextComponent*>(previous.get());
                if (prevTextComponent) {
                    std::cout << "Component::layout - previous [" << prevTextComponent->text << "] ending at: " << (int)prevTextComponent->endingX << "x" << (int)prevTextComponent->endingY << "" << std::endl;
                }
                */
                //std::cout << "Component::layout - previous at: " << (int)previous->x << "x" << (int)previous->y << " size: " << (int)previous->width << "x" << (int)previous->height << " ending at: " << (int)previous->endingX << "x" << (int)previous->endingY << std::endl;
                // 2nd or last
                if (previous->isInline) {
                    // last was inline
                    if (isInline) {
                        x = previous->x + previous->width;
                        y = previous->y; // keep on same line
                        //std::cout << "Component::layout - inLine (" << (int)previous->width << " wide) inLine" << std::endl;
                        if (x >= windowWidth) {
                            //std::cout << "Component::layout - inline inline wrapping because x: " << (int)x << " window: " << windowWidth << std::endl;
                            x = previous->endingX;
                            //std::cout << "Component::layout - p.y: " << (int)previous->y << " p.ey: " << previous->endingY << " p.h" << (int)previous->height << std::endl;
                            y = previous->y - previous->height + previous->endingY;
                        }
                    } else {
                        // we're block
                        y = previous->y - previous->height;
                        //std::cout << "Component::layout - inLine block" << std::endl;
                    }
                } else {
                    // last was block
                    y = previous->y - previous->height;
                    //std::cout << "Component::layout - block *  p.x: " << (int)previous->x << " p.h: " << (int)previous->height << std::endl;
                }
                // really only inline but can't hurt block AFAICT
                if (x >= windowWidth) {
                    //std::cout << "Component::layout - wrapping because x: " << (int)x << " window: " << windowWidth << std::endl;
                    x = 0;
                    y -= previous->height; // how far down do we need to wrap?, the previous height?
                }
            } else {
                // first, there will be no width to add
            }
        } else {
            // first component for this parent
        }
    }
    //std::cout << "Component::layout - adjusted by prev: " << (int)x << "x" << (int)y << std::endl;
    //std::cout << "Component::layout - moving component to " << (int)x << "x" << (int)y << std::endl;
    
    // change in X position
    //std::cout << "Component::layout - component was at " << lx << " moved(?) to " << (int)x << std::endl;

    // also need to update texture if our width (which is calculated from windowWidth) changes
    // it's more than width because a change to windowWidth fucks up all coordinates in it
    // so it has to be recalculated (??)
    //if (x!=lx || !textureSetup) {
        // this line may have more or less room
        // we'll need to rewrap component
        wrap();
    //}

    // recurse down over my children and update their position
    // if parent x or y changes or windowWindow changes, all children need to be adjust
    //std::cout << "Component::layout - I have " << children.size() << " children affected by my moved" << std::endl;
    for (std::shared_ptr<Component> child : children) {
        // update new sizes
        child->windowWidth = windowWidth;
        child->windowHeight = windowHeight;
        // update it's layout
        child->layout();
    }
    // also maybe bump siblings after us (inline if x/y moved, block if y moved)
}

// resize/wordwrap to available width
// our position is required
void Component::wrap() {
    float lW = width;
    float lH = height;
    resize(windowWidth, windowHeight); // this may change our w/h
    textureSetup = true;
    if (lW != width || lH != height) {
        //std::cout << "Component::wrap - component was " << (int)lW << "x" << (int)lH << " now " << (int)width << "x" << (int)height << std::endl;
        // size has change, update our parent
        updateParentSize();
    } else {
        //std::cout << "Component::wrap - component same size" << std::endl;
    }
}

// measure current, apply changes to parent
// our size is required
void Component::updateParentSize() {
    if (!parent) {
        //std::cout << "Component::updateParentSize - can't update parent size, no parent" << std::endl;
        return;
    }
    // back up current size
    unsigned int lastParentWidth = parent->width;
    unsigned int lastParentHeight = parent->height;
    
    parent->endingX = endingX;
    parent->endingY = endingY;
    
    // find max width of all siblings
    unsigned int maxWidth = width; // float?
    unsigned int heightAccum = 0;
    bool wasInline =  false;
    unsigned int totalHeight = 0;

    // integrity check
    if (!parent->children.size()) {
        // currently means creation
        std::cout << "Component::updateParentSize - parent's has no children, strange since we are one" << std::endl;
        totalHeight = height;
    }
    
    // look at siblings
    for (std::shared_ptr<Component> child : parent->children) {
        maxWidth = std::max(maxWidth, static_cast<unsigned int>(child->width));
        if (child->isInline) {
            heightAccum = std::max(heightAccum, static_cast<unsigned int>(child->height));
            /*
            if (wasInline) {
                // in in
                heightAccum = std::max(heightAccum, static_cast<int>(child->height));
            } else {
                // bl in
                heightAccum = static_cast<int>(child->height);
            }
            */
        } else {
            if (wasInline) {
                // in bl
                // flush height of previous line + our height
                totalHeight += heightAccum + child->height;
                heightAccum = 0; // reset
            } else {
                // bl bl
                totalHeight += child->height;
            }
        }
        wasInline = child->isInline;
    }
    // flush any remaining heightAccum
    totalHeight += heightAccum;
    
    //std::cout << "new size " << maxWidth << "x" << totalHeight << std::endl;
    // did our size actually change
    if (lastParentWidth != maxWidth || lastParentHeight != totalHeight) {
        //std::cout << "Component::updateParentSize - from " << (int)height << " totalHeight: " << totalHeight << std::endl;
        parent->width = maxWidth;
        parent->height = totalHeight;
        parent->updateParentSize(); // our parent size can only be recalculated by looking at it's children
    }
}

GLuint elementBufferObject = 0;
void Component::initTheElementBufferObject() const {
    glGenBuffers(1, &elementBufferObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

GLuint Component::CreateVertexArrayObject() const {
    GLuint vertexArrayObject = 0;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
    // we may need the VBO here...

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //glBindVertexArray(0);
    return vertexArrayObject;
}

GLuint Component::CreateVertexBufferObject() const {
    GLuint vertexBufferObject = 0;
    glGenBuffers(1, &vertexBufferObject);
    return vertexBufferObject;
}

// this always creates, we probably should just respond to existing state
bool Component::setPosition4(GLuint vertexBufferObject, int x0, int y0, int x1, int y1) const {
    float vertices[20] = {
        0.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f
    };
    
    vertices[(0 * 5) + 0] = x0;
    vertices[(0 * 5) + 1] = y1;
    
    vertices[(1 * 5) + 0] = x1;
    vertices[(1 * 5) + 1] = y1;
    
    vertices[(2 * 5) + 0] = x1;
    vertices[(2 * 5) + 1] = y0;
    
    vertices[(3 * 5) + 0] = x0;
    vertices[(3 * 5) + 1] = y0;
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // selects buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // writes buffer
    
    return true;
}

GLuint Component::CreateTexture() const {
    GLuint textureNum = 0;
    glGenTextures(1, &textureNum);
    return textureNum;
}

bool Component::setTexture(GLuint textureNum, GLsizei w, GLsizei h, const unsigned char *texture) const {
    glBindTexture(GL_TEXTURE_2D, textureNum);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    return true;
}

// converts 0-1 to screen
// but centered
void Component::pointToViewport(float &rawX, float &rawY) const {
    if (boundToPage) {
        //std::cout << "Component::pointToViewport - notBoundToPage converting from " << static_cast<int>(rawX) << "," << static_cast<int>(rawY) << std::endl;
        //std::cout << "BoundToPage using " << screenWidth << "x" << screenHeight << std::endl;
        rawX = ((rawX / windowWidth) * 2) - 1;
        rawY = ((rawY / windowHeight) * 2) - 1;
        //std::cout << "Component::pointToViewport - BoundToPage using " << static_cast<int>(rawX) << "x" << static_cast<int>(rawY) << std::endl;
        //std::cout << "Component::pointToViewport - BoundToPage converted to " << rawX << "," << rawY << std::endl;
    } else {
        //std::cout << "notBoundToPage using " << screenWidth << "x" << screenHeight << std::endl;
        //std::cout << "Component::pointToViewport - notBoundToPage converting from " << static_cast<int>(rawX) << "," << static_cast<int>(rawY) << std::endl;
        if (rawX < 0) {
            rawX += windowWidth;
        }
        if (rawY < 0) {
            rawY += windowHeight;
        }
        if (rawX > 1) {
            rawX /= windowWidth;
        }
        if (rawY > 1) {
            rawY /= windowHeight;
        }
        rawX = (rawX * 2) - 1;
        rawY = (rawY * 2) - 1;
        //std::cout << "Component::pointToViewport - notBoundToPage converted to " << rawX << "," << rawY << std::endl;
    }
}

// keeps 0-1 (and *2 to convert to screen)
// but also takes pixels (and converts to screen)
// anchors to upperleft
void Component::distanceToViewport(float &rawX, float &rawY) const {
    if (std::abs(rawX) > 1) {
        rawX /= windowWidth;
    }
    if (std::abs(rawY) > 1) {
        rawY /= windowHeight;
    }
    rawX *= 2;
    rawY *= 2;
}
