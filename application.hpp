#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "window.hpp"
#include "renderer.hpp"
#include "scene_manager.hpp"

class Application {
    Window window;
    Renderer renderer;
    SceneManager sceneManager;

    bool CreateConsole();

    void Update(float deltaTime);
    void Render();

public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
};

#endif