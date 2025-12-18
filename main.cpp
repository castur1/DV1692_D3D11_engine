#include <Windows.h>

#include "application.hpp"
#include "logging.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    Application app;

    if (!app.Initialize())
        return -1;

    app.Run();

    return 0;
}

/* TODO:
 *  - Window resizing. Recreate the Render Target View and such when the WM_SIZE is caught?
 *  - Add the debug code to the end of wWinMain
 *  - Better error handling
 */