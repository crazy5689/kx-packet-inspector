#pragma once

#include "PacketHeaders.h" // For PacketHeader in filter map
#include <map>
#include <atomic>
#include <cstdint> // For uintptr_t

namespace kx {

// --- Status Information ---
enum class HookStatus {
    Unknown,
    OK,
    Failed
};

extern HookStatus g_presentHookStatus;
extern HookStatus g_msgSendHookStatus;
extern HookStatus g_msgRecvHookStatus; // Placeholder
extern uintptr_t g_msgSendAddress;
extern uintptr_t g_msgRecvAddress;  // Placeholder

// --- UI State ---
extern bool g_isInspectorWindowOpen; // Controls main loop / unload trigger
extern bool g_showInspectorWindow;   // Controls GUI visibility (toggle via hotkey)
extern bool g_capturePaused;         // Controls packet capture logging

// --- Filtering State ---
enum class FilterMode {
    ShowAll,
    IncludeOnly, // Show only checked items
    Exclude      // Hide checked items
};

extern FilterMode g_packetFilterMode;
extern std::map<PacketHeader, bool> g_packetFilterSelection; // Map header -> selected state

// --- Shutdown Synchronization ---
extern std::atomic<bool> g_isShuttingDown; // Flag to signal shutdown to hooks

}