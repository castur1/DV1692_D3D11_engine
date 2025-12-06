#include <Windows.h>

#include "application.hpp"
#include "logging.hpp"

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance, 
    _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPWSTR lpCmdLine, 
    _In_ int nCmdShow
) {
    Application app;
    if (!app.Initialize())
        return -1;

    app.Run();

    return 0;
}