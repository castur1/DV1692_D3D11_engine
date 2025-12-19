#include "window.hpp"
#include "logging.hpp"

Window::Window() : shouldClose(true), hInst{}, hWnd{} {}

Window::~Window() {
    if (this->hWnd) DestroyWindow(this->hWnd);
    if (this->hInst) UnregisterClass(L"window_class", this->hInst);
}

bool Window::Initialize(int width, int height, const wchar_t *title) {
    LogInfo(" > Creating window...\n");

    this->hInst = GetModuleHandle(nullptr);

    WNDCLASS windowClass{};
    windowClass.lpfnWndProc = this->WindowProc;
    windowClass.hInstance = this->hInst;
    windowClass.lpszClassName = L"window_class";
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&windowClass);

    this->hWnd = CreateWindowEx(
        0, 
        windowClass.lpszClassName, 
        title, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        width, height, 
        NULL, 
        NULL, 
        this->hInst, 
        this
    );

    if (!this->hWnd) {
        LogError("Failed to create window\n");
        return false;
    }

    ShowWindow(this->hWnd, SW_SHOWDEFAULT);

    ShowCursor(false); // TODO

    this->shouldClose = false;

    LogInfo(" > Window created\n");

    return true;
}

void Window::ProcessMessages() {
    MSG message{};
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

bool Window::ShouldClose() {
    return this->shouldClose;
}

HWND Window::GetHandle() const {
    return this->hWnd;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window *window{};
    if (uMsg == WM_CREATE) {
        window = (Window *)((CREATESTRUCT *)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    } else {
        window = (Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    switch (uMsg) {
        case WM_QUIT:
        case WM_CLOSE: {
            window->shouldClose = true;
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
