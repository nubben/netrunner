#include "Component.h"
#include <iostream>

Component::~Component() {
}

void Component::resize() {
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
    
    // reset position
    x = 0;
    y = 0;
    
    // if we're a child, get our parents position
    if (parent) {
        //std::cout << "Component::layout - copying position from parent: " << (int)parent->x << "x" << (int)parent->y << std::endl;
        x = parent->x;
        y = parent->y;
        // if we have sibilings see if they're inline or block
        if (parent->children.size()) {
            //std::cout << "Component::layout - parent children: " << parent->children.size() << std::endl;
            if (previous) {
                // 2nd or last
                if (previous->isInline) {
                    // last was inline
                    if (isInline) {
                        x = previous->x + previous->width;
                        y = previous->y; // keep on same line
                        //std::cout << "Component::layout - inLine (" << (int)previous->width << " wide) inLine" << std::endl;
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
                if (x == windowWidth) {
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
    //std::cout << "moving component to " << (int)x << "x" << (int)y << std::endl;
    
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
    resize(); // this may change our w/h
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
