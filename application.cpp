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

    if (!this->window.Initialize(1280, 720, L"DV1692 D3D11 engine demo"))
        return false;
    
    if (!this->renderer.Initialize(this->window.GetHandle()))
        return false;

    LogInfo("Intialization successful!\n");

    this->assetManager.Initialize(this->renderer.GetDevice());

    this->sceneManager.Initialize(&this->renderer, &this->assetManager);

    return true;
}

void Application::Run() {
    LogInfo("Running...\n");

    auto previousTime = std::chrono::high_resolution_clock::now();

    Input::CaptureMouse(this->window.GetHandle());

    while (!this->window.ShouldClose()) {
        this->window.ProcessMessages();

        if (this->window.WasResized()) {
            this->renderer.Resize(this->window.GetWidth(), this->window.GetHeight());
            this->window.ClearResizeFlag();
        }

        if (Input::IsKeyPressed(VK_F4))
            this->renderer.ToggleFullscreen();

        if (Input::IsKeyPressed(VK_ESCAPE))
            Input::ReleaseMouse();

        if (!Input::IsMouseCaptured() && Input::IsKeyPressed(VK_LBUTTON)) {
            HWND hWnd = this->window.GetHandle();
            HWND foregroundHWnd = GetForegroundWindow();

            if (hWnd == foregroundHWnd) {
                POINT cursorPos;
                if (GetCursorPos(&cursorPos)) {
                    ScreenToClient(hWnd, &cursorPos);

                    RECT clientRect;
                    GetClientRect(hWnd, &clientRect);

                    if (cursorPos.x >= 0 && cursorPos.x < clientRect.right &&
                        cursorPos.y >= 0 && cursorPos.y < clientRect.bottom) {
                        Input::CaptureMouse(this->window.GetHandle());
                    }
                }
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - previousTime;
        previousTime = currentTime;

        float deltaTime = elapsed.count();

        this->Update(deltaTime);
        this->Render();
    }

    LogInfo("Shutdown...\n");
}
