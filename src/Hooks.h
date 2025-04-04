#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include "../MinHook/MinHook.h"
#if _WIN64
#pragma comment(lib, "MinHook/libMinHook.x64.lib")
#else
#pragma comment(lib, "MinHook/libMinHook.x86.lib")
#endif

bool InitializeHooks();
void CleanupHooks();
