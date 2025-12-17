#include <Windows.h>
#include <stdio.h>

#include "application.hpp"
#include "logging.hpp"

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

void Application::Update() {
    // TODO: deltaTime
    this->sceneManager.Update(0.01667f);
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

    LogInfo("Intialization successful!\n");

    return true;
}

void Application::Run() {
    LogInfo("Running...\n");

    while (!this->window.ShouldClose()) {
        this->window.ProcessMessages();

        this->Update();
        this->Render();
    }

    LogInfo("Shutdown...\n");
}
