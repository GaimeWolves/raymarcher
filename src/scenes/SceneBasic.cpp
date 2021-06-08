#include <scenes/SceneBasic.hpp>

#include <memory>
#include <filesystem>

#include <util.hpp>
#include <defs.hpp>

#include <engine/shader.hpp>
#include <engine/LookAtCamera.hpp>

static bool mouseDown = false;
static glm::vec2 mousePos;

// Rendering stuff
GLuint vao;
GLuint vbo;

std::unique_ptr<Shader> shader;

// Camera stuff
Engine::LookAtCamera camera = Engine::LookAtCamera(M_PI_2);

void SceneBasic::init()
{
    shader = std::make_unique<Shader>(
        (execPath + "shaders/empty.vert").c_str(),
        (execPath + "shaders/marching.frag").c_str());

    shader->createUniform("screenSize");
    shader->createUniform("cameraPosition");
    shader->createUniform("fov");
    shader->createUniform("rotationMatrix");

    static const float vertices[] = {
        -1, -1, 0,
        -1, 1, 0,
        1, -1, 0,
        1, 1, 0};

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    reset();
}

void SceneBasic::reset()
{
    camera.set_position(glm::vec3(0, 0, -2));
    camera.set_pitch(0.3f);
    camera.set_yaw(0.0f);

    shader->bind();
    shader->loadVector2fUniform(screenSize, "screenSize");
    shader->loadVector3fUniform(camera.get_position(), "cameraPosition");
    shader->loadFloatUniform(camera.get_fov(), "fov");
    shader->loadMatrix4fUniform(camera.get_rotation(), "rotationMatrix");
    shader->unbind();
}

void SceneBasic::handle_events(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_r)
        {
            shader = std::make_unique<Shader>(
                (execPath + "../src/shaders/empty.vert").c_str(),
                (execPath + "../src/shaders/marching.frag").c_str());

            shader->createUniform("screenSize");
            shader->createUniform("cameraPosition");
            shader->createUniform("fov");
            shader->createUniform("rotationMatrix");

            shader->bind();
            shader->loadVector2fUniform(screenSize, "screenSize");
            shader->loadVector3fUniform(camera.get_position(), "cameraPosition");
            shader->loadFloatUniform(camera.get_fov(), "fov");
            shader->loadMatrix4fUniform(camera.get_rotation(), "rotationMatrix");
            shader->unbind();
        }
    }

    camera.handleEvent(event);
}

void SceneBasic::update(double dt)
{
    camera.update();
}

void SceneBasic::render()
{
    shader->bind();

    shader->loadVector3fUniform(camera.get_position(), "cameraPosition");
    shader->loadMatrix4fUniform(camera.get_rotation(), "rotationMatrix");

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    shader->unbind();
}