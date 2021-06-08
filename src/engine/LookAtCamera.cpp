#include <engine/LookAtCamera.hpp>

#include <algorithm>

namespace Engine
{
    static bool mouseLeft = false;
    static bool mouseRight = false;

    void LookAtCamera::handleEvent(SDL_Event event)
    {
        switch (event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT && !mouseRight)
            {
                mouseLeft = true;
                SDL_WarpMouseInWindow(window, screenSize.x / 2, screenSize.y / 2);
            }
            else if (event.button.button == SDL_BUTTON_RIGHT && !mouseLeft)
            {
                mouseRight = true;
                SDL_WarpMouseInWindow(window, screenSize.x / 2, screenSize.y / 2);
            }

            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                mouseLeft = false;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                mouseRight = false;

            break;
        case SDL_MOUSEMOTION:
            if (mouseLeft || mouseRight)
            {
                glm::vec2 delta = glm::vec2(screenSize.x / 2 - event.motion.x, screenSize.y / 2 - event.motion.y);

                if (mouseLeft)
                {
                    m_yaw += delta.x * 0.0025f;
                    m_pitch = std::clamp(m_pitch + delta.y * 0.0025f, -M_PI_2f32 + 0.01f, M_PI_2f32 - 0.01f);
                }
                else if (mouseRight)
                    m_position += m_front * delta.y * 0.005f - m_right * delta.x * 0.005f;

                SDL_WarpMouseInWindow(window, screenSize.x / 2, screenSize.y / 2);
            }

            break;
        case SDL_MOUSEWHEEL:
            m_position += m_direction * (float)event.wheel.y * 0.1f;
            break;
        }
    }

    void LookAtCamera::update()
    {
        updateView();
    }

    void LookAtCamera::updateView()
    {
        m_direction = glm::vec3(cos(m_pitch) * sin(m_yaw), sin(m_pitch), cos(m_pitch) * cos(m_yaw));
        m_right = glm::vec3(sin(m_yaw - M_PI_2), 0, cos(m_yaw - M_PI_2));
        m_front = glm::vec3(sin(m_yaw), 0, cos(m_yaw));
        m_up = glm::cross(m_right, m_direction);

        m_rotation = glm::lookAt(m_position, m_position + m_direction, m_up);
    }
} // namespace Engine
