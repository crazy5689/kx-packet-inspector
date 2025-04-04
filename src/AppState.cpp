#include "AppState.h"
#include <map>      // For std::map definition
#include <atomic>   // For std::atomic definition
#include "PacketHeaders.h" // For PacketHeader definition

namespace kx {

// --- Status Information ---
HookStatus g_presentHookStatus = HookStatus::Unknown;
HookStatus g_msgSendHookStatus = HookStatus::Unknown;
HookStatus g_msgRecvHookStatus = HookStatus::Unknown; // Placeholder
uintptr_t g_msgSendAddress = 0;
uintptr_t g_msgRecvAddress = 0;  // Placeholder

// --- UI State ---
bool g_isInspectorWindowOpen = true; // Controls main loop / unload trigger
bool g_showInspectorWindow = true;   // Controls GUI visibility, start visible
bool g_capturePaused = false;        // Start with capture enabled

// --- Filtering State ---
FilterMode g_packetFilterMode = FilterMode::ShowAll;
std::map<PacketHeader, bool> g_packetFilterSelection; // Initialized empty, populated in ImGuiManager::Initialize

// --- Shutdown Synchronization ---
std::atomic<bool> g_isShuttingDown = false;

}