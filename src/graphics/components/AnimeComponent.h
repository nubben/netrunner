#ifndef ANIMECOMPONENT_H
#define ANIMECOMPONENT_H

#include <GL/glew.h>
#include "BoxComponent.h"

class AnimeComponent : public BoxComponent {
public:
    AnimeComponent(const float rawX, const float rawY, const float rawWidth, const float rawHeight, const int windowWidth, const int windowHeight);
};

#endif