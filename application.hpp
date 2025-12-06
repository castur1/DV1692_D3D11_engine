#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "window.hpp"
#include "renderer.hpp"

class Application {
    Window window;
    Renderer renderer;

    bool CreateConsole();

    void Update();
    void Render();

public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
};

#endif