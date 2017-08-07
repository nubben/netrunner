#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include <GL/glew.h>
#include <string>
#include "BoxComponent.h"
#include "TextComponent.h"

class InputComponent : public BoxComponent {
public:
    //: BoxComponent(rawX, rawY, rawWidth, rawHeight, passedWindowWidth, passedWindowHeight) { }
    InputComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int passedWindowWidth, const int passedWindowHeight);
    // or
    //using BoxComponent::BoxComponent;
    void resize(const int passedWindowWidth, const int passedWindowHeight);
    void render();
    void addChar(char c);
    void backSpace();
    void updateText();
    std::string value="";
    TextComponent *userInputText = nullptr;
};

#endif
