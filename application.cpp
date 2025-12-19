#include <Windows.h>
#include <stdio.h>
#include <chrono>

#include "application.hpp"
#include "logging.hpp"
#include "input.hpp"

Application::Application() {}

Application::~Application() {}

bool Application::CreateConsole() {
    if (!AllocConsole())
        return false;

    FILE *file;
    freopen_s(&file, "CONOUT$", "w", stdout);
    freopen_s(&file, "CONIN$", "r", stdin);

    return true;
}

void Application::Update(float deltaTime) {
    Input::Update();
    this->sceneManager.Update(deltaTime);
}

void Application::Render() {
    this->renderer.Begin();

    this->sceneManager.Render(&this->renderer);
    this->renderer.Flush();

    this->renderer.End();
}

bool Application::Initialize() {
#ifdef _DEBUG
    if (!this->CreateConsole())
        return false;
#endif

    LogInfo("Initializing...\n");

    if (!this->window.Initialize(1280, 720, L"Hello, World!"))
        return false;
    
    if (!this->renderer.Initialize(this->window.GetHandle()))
        return false;

    this->assetManager.Initialize(this->renderer.GetDevice());

    this->sceneManager.Initialize(&this->renderer, &this->assetManager);

    LogInfo("Intialization successful!\n");

    return true;
}

void Application::Run() {
    LogInfo("Running...\n");

    auto previousTime = std::chrono::high_resolution_clock::now();

    while (!this->window.ShouldClose()) {
        this->window.ProcessMessages();

        if (Input::IsKeyPressed(VK_ESCAPE))
            break;

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - previousTime;
        previousTime = currentTime;

        float deltaTime = elapsed.count();

        this->Update(deltaTime);
        this->Render();
    }

    LogInfo("Shutdown...\n");
}
