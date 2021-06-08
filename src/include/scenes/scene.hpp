#pragma once

#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

class Scene
{
public:
    virtual void init() = 0;
    virtual void reset() = 0;
    virtual void handle_events(SDL_Event event) = 0;
    virtual void update(double dt) = 0;
    virtual void render() = 0;
};