#pragma once

#include <scenes/scene.hpp>

class SceneBasic final : public Scene
{
    void init() override;
    void reset() override;
    void handle_events(SDL_Event event) override;
    void update(double dt) override;
    void render() override;
};