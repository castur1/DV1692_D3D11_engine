#include <Windows.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "application.hpp"
#include "logging.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    {
        Application app;

        if (!app.Initialize())
            return -1;

        app.Run();
    }

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

    OutputDebugStringW(L"\n[_CrtDumpMemoryLeaks start]\n");
    _CrtDumpMemoryLeaks();
    OutputDebugStringW(L"\n[_CrtDumpMemoryLeaks end]\n\n");
    
    return 0;
}

/* TODO:
 *  - Window resizing. Recreate the Render Target View and such when the WM_SIZE is caught?
 *  - Better error handling
 */