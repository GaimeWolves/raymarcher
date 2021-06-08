#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <defs.hpp>

static float angleBetween(glm::vec2 a, glm::vec2 b, glm::vec2 origin)
{
    glm::vec2 da = glm::normalize(a - origin);
    glm::vec2 db = glm::normalize(b - origin);
    return glm::acos(glm::dot(da, db));
}

static glm::vec2 map(glm::vec2 value, glm::vec2 inMin, glm::vec2 inMax, glm::vec2 outMin, glm::vec2 outMax)
{
    return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

// Some extensions
namespace glm
{
    float cross(const vec2 &a, const vec2 &b);
    vec2 cross(const vec2 &a, const float s);
    vec2 cross(const float s, const vec2 &a);
}