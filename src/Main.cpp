#include <iostream>
#include <cstdio> // Required for fclose
#include <windows.h>
#include "Console.h"
#include "Hooks.h"
#include "AppState.h"   // Include for g_isInspectorWindowOpen, g_isShuttingDown

HINSTANCE dll_handle;

// Eject thread to free the DLL
DWORD WINAPI EjectThread(LPVOID lpParameter) {
#ifdef _DEBUG
    // In Debug builds, close standard streams and free the console
    if (stdout) fclose(stdout);
    if (stderr) fclose(stderr);
    if (stdin) fclose(stdin);

    if (!FreeConsole()) {
        OutputDebugStringA("kx-packet-inspector: FreeConsole() failed.\n");
    }
#endif // _DEBUG
    Sleep(100);
    FreeLibraryAndExitThread(dll_handle, 0);
    return 0;
}

// Main function that runs in a separate thread
DWORD WINAPI MainThread(LPVOID lpParameter) {
#ifdef _DEBUG
    kx::SetupConsole(); // Only setup console in Debug builds
#endif // _DEBUG

    if (!InitializeHooks()) {
        std::cerr << "Failed to initialize hooks." << std::endl;
        return 1;
    }

    // Main loop to keep the hook alive
    while (kx::g_isInspectorWindowOpen && !(GetAsyncKeyState(VK_DELETE) & 0x8000)) {
        Sleep(100); // Sleep to avoid busy-waiting
    }

    // Signal hooks to stop processing before actual cleanup
    kx::g_isShuttingDown = true;
    // Add a small delay to allow the render thread to potentially finish its current frame
    Sleep(100); // Adjust delay if needed, or consider more robust synchronization

    // Cleanup hooks and ImGui
    CleanupHooks();

    // Eject the DLL and exit the thread
    CreateThread(0, 0, EjectThread, 0, 0, 0);

    return 0;
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        dll_handle = hModule;
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        // Optional: Handle cleanup if necessary
        break;
    }
    return TRUE;
}
