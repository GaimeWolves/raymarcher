#pragma once

#include <util.hpp>
#include <defs.hpp>

namespace Engine
{
    class LookAtCamera
    {
    public:
        LookAtCamera(float fov)
            : m_fov(fov)
        {
            update();
        }

        void handleEvent(SDL_Event event);
        void update();

        void set_position(glm::vec3 position) { m_position = position; }
        void set_pitch(float pitch) { m_pitch = pitch; }
        void set_yaw(float yaw) { m_yaw = yaw; }

        const glm::vec3 &get_position() const { return m_position; }
        const glm::mat4 &get_rotation() const { return m_rotation; }
        const float get_fov() const { return m_fov; }

    private:
        void updateView();

        float m_fov{M_PI_2};
        float m_pitch{0.3};
        float m_yaw{0};
        glm::vec3 m_position{};
        glm::vec3 m_right{};
        glm::vec3 m_front{};
        glm::vec3 m_up{};
        glm::vec3 m_direction{};
        glm::mat4 m_rotation{};
    };
} // namespace Engine