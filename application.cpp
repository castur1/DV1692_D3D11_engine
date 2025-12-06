#include <Windows.h>
#include <stdio.h>

#include "application.hpp"
#include "logging.hpp"

Application::Application() {}

Application::~Application() {}

bool Application::Initialize() {
#ifdef _DEBUG
    if (!this->CreateConsole())
        return false;
#endif

    LogInfo("Initializing...\n");

    if (!this->window.Initialize(1280, 720, L"Hello, World!"))
        return false;
    
    if (!this->renderer.Initialize())
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

bool Application::CreateConsole() {
    if (!AllocConsole()) {
        LogError("Failed to create the console\n");
        return false;
    }

    FILE *file;
    freopen_s(&file, "CONOUT$", "w", stdout);
    freopen_s(&file, "CONIN$", "r", stdin);

    return true;
}

void Application::Update() {}

void Application::Render() {}
