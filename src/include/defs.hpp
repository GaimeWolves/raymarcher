#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <string>

#ifdef _WIN32
#define M_PI 3.1415926535897932
#define M_PI_2 1.5707963267948966
#define M_PI_4 0.7853981633974483
#endif

extern const glm::vec2 screenSize;
extern SDL_Window *window;
extern std::string execPath;