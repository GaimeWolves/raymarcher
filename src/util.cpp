#include <util.hpp>

// Some extensions
namespace glm
{
    float cross(const vec2 &a, const vec2 &b)
    {
        return a.x * b.y - a.y * b.x;
    }

    vec2 cross(const vec2 &a, const float s)
    {
        return vec2(s * a.y, -s * a.x);
    }

    vec2 cross(const float s, const vec2 &a)
    {
        return vec2(-s * a.y, s * a.x);
    }
} // namespace glm