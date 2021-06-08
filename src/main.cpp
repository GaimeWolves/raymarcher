#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cmath>
#include <numbers>

#include <vector>
#include <deque>
#include <memory>
#include <filesystem>

#include <engine/shader.hpp>

#include <scenes/SceneBasic.hpp>
#include <defs.hpp>

std::string execPath;

SDL_Window *window;

static const int width = 800;
static const int height = 800;

static uint64_t NOW = 0;
static uint64_t LAST = 0;
static double deltaTime = 0;

const glm::highp_vec2 screenSize((float)width, (float)height);

static size_t currentScene = 0;
static std::unique_ptr<Scene> scenes[] = {
    std::make_unique<SceneBasic>(),
};

static void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    std::cout << "[OpenGL Error] " << message << std::endl;
}

static void init()
{
    NOW = SDL_GetPerformanceCounter();

    SDL_WarpMouseInWindow(window, width / 2, height / 2);

    for (auto &scene : scenes)
        scene->init();
}

static void handleEvents(SDL_Event event)
{
    scenes[currentScene]->handle_events(event);
}

static void update(double dt)
{
    scenes[currentScene]->update(dt);
}

static void render()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scenes[currentScene]->render();
}

int main(int argc, char **argv)
{
    execPath = std::filesystem::path(std::string(argv[0])).remove_filename().string();

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Doppelpendel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openGLDebugCallback, 0);
#endif

    execPath = std::filesystem::path(std::string(argv[0])).remove_filename().string();

    init();

    bool close = false;

    while (!close)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                close = true;
            }

            handleEvents(event);
        }

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

        SDL_SetWindowTitle(window, (std::to_string(1 / deltaTime) + " FPS").c_str());

        update(deltaTime);
        render();

        SDL_GL_SwapWindow(window);
    }

    return 0;
}