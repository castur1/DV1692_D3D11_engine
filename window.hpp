#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <Windows.h>

class Window {
    bool shouldClose;

    HINSTANCE hInst;
    HWND hWnd;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    Window();
    ~Window();

    bool Initialize(int width, int height, const wchar_t *title);
    void ProcessMessages();
    bool ShouldClose();
    HWND GetHandle() const;
};

#endif