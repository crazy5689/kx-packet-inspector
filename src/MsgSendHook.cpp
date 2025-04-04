#include "MsgSendHook.h"
#include "Config.h"       // For potential config values if used later
#include "PacketData.h"   // For PacketInfo, g_packetLog, g_packetLogMutex
#include "AppState.h"     // For g_capturePaused, g_isShuttingDown
#include "PacketHeaders.h"// For PacketHeader enum and helpers
#include "GameStructs.h"  // For MsgSendContext structure definition

#include <vector>
#include <chrono>
#include <stdexcept>
#include <limits>   // For numeric_limits
#include <iostream> // For temporary error logging (replace with a proper logger later)

// Pointer to the original game function that we detour.
MsgSendFunc originalMsgSend = nullptr;
// Address of the hooked function, stored for cleanup.
static uintptr_t hookedMsgSendAddress = 0;

// Detour function for the game's internal message sending logic.
// Intercepts outgoing packets before potential encryption/final processing.
void __fastcall hookMsgSend(void* param_1) {

    if (param_1 == nullptr) {
        // Basic sanity check; call original if possible even on invalid input?
        if (originalMsgSend) originalMsgSend(param_1);
        return;
    }

    // Proceed only if capturing is enabled and the application isn't shutting down.
    bool shouldLogPacket = !kx::g_capturePaused && !kx::g_isShuttingDown.load();

    if (shouldLogPacket) {
        try {
            auto* context = reinterpret_cast<kx::GameStructs::MsgSendContext*>(param_1);

            // Skip logging if bufferState indicates the buffer might be invalid or getting reset.
            // This condition (state == 1) is based on analysis of the original function.
            if (context->bufferState == 1) {
                shouldLogPacket = false;
            }

            if (shouldLogPacket) {
                std::uint8_t* packetData = context->GetPacketBufferStart();
                std::size_t bufferSize = context->GetCurrentDataSize();

                // --- Perform Sanity Checks Before Data Access ---
                bool dataIsValid = true;
                if (packetData == nullptr || context->currentBufferEndPtr == nullptr) {
                    std::cerr << "[Error] Null pointer in MsgSendContext." << std::endl;
                    dataIsValid = false;
                }
                else if (context->currentBufferEndPtr < packetData) {
                    std::cerr << "[Error] Invalid packet buffer pointers (end < start)." << std::endl;
                    dataIsValid = false;
                }
                else {
                    // Limit size to prevent reading excessive memory if pointers are wrong.
                    const std::size_t MAX_REASONABLE_PACKET_SIZE = 16 * 1024;
                    if (bufferSize > MAX_REASONABLE_PACKET_SIZE) {
                        std::cerr << "[Error] Packet size (" << bufferSize << ") exceeds sanity limit (" << MAX_REASONABLE_PACKET_SIZE << ")." << std::endl;
                        dataIsValid = false;
                    }
                    // Check against the destination integer type limit.
                    else if (bufferSize > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
                        std::cerr << "[Error] Packet size exceeds std::numeric_limits<int>::max()." << std::endl;
                        dataIsValid = false;
                    }
                }
                // --- End Sanity Checks ---

                if (dataIsValid && bufferSize > 0) {
                    kx::PacketInfo info;
                    info.timestamp = std::chrono::system_clock::now();
                    info.size = static_cast<int>(bufferSize); // Safe cast after checks
                    info.direction = kx::PacketDirection::Sent;

                    info.data.assign(packetData, packetData + bufferSize);

                    std::uint8_t rawHeader = info.data[0];
                    info.header = kx::ToPacketHeader(rawHeader);
                    info.name = kx::GetPacketName(rawHeader);

                    {
                        std::lock_guard<std::mutex> lock(kx::g_packetLogMutex);
                        kx::g_packetLog.push_back(std::move(info));
                        // TODO: Consider adding log size limiting logic here.
                    }
                }
            }

        }
        catch (const std::exception& e) {
            std::cerr << "[hookMsgSend] Exception during packet capture: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "[hookMsgSend] Unknown exception during packet capture." << std::endl;
        }
    }

    // Always pass the call to the original function.
    if (originalMsgSend) {
        originalMsgSend(param_1);
    }
}

// Initializes the hook for the MsgSend function at the given address.
bool InitializeMsgSendHook(uintptr_t targetFunctionAddress) {
    if (targetFunctionAddress == 0) {
        std::cerr << "[Error] InitializeMsgSendHook called with null address." << std::endl;
        return false;
    }

    // Store address for potential cleanup before attempting hook creation.
    hookedMsgSendAddress = targetFunctionAddress;

    auto createStatus = MH_CreateHook(reinterpret_cast<void*>(targetFunctionAddress), &hookMsgSend, reinterpret_cast<void**>(&originalMsgSend));
    if (createStatus != MH_OK) {
        std::cerr << "[Error] Failed to create MsgSend hook. Status: " << MH_StatusToString(createStatus) << std::endl;
        hookedMsgSendAddress = 0; // Invalidate stored address
        return false;
    }

    auto enableStatus = MH_EnableHook(reinterpret_cast<void*>(targetFunctionAddress));
    if (enableStatus != MH_OK) {
        std::cerr << "[Error] Failed to enable MsgSend hook. Status: " << MH_StatusToString(enableStatus) << std::endl;
        MH_RemoveHook(reinterpret_cast<void*>(targetFunctionAddress)); // Attempt cleanup
        hookedMsgSendAddress = 0;
        return false;
    }

    std::cout << "[Info] MsgSend hook initialized successfully at 0x" << std::hex << targetFunctionAddress << std::dec << std::endl;
    return true;
}

// Disables and removes the MsgSend hook.
void CleanupMsgSendHook() {
    if (hookedMsgSendAddress != 0) {
        // Attempt to disable and remove the hook, logging potential errors.
        auto disableStatus = MH_DisableHook(reinterpret_cast<void*>(hookedMsgSendAddress));
        auto removeStatus = MH_RemoveHook(reinterpret_cast<void*>(hookedMsgSendAddress));

        if (disableStatus != MH_OK) {
            std::cerr << "[Warning] Failed to disable MsgSend hook. Status: " << MH_StatusToString(disableStatus) << std::endl;
        }
        if (removeStatus != MH_OK) {
            std::cerr << "[Warning] Failed to remove MsgSend hook. Status: " << MH_StatusToString(removeStatus) << std::endl;
        }

        // Clear references even if MH operations failed.
        hookedMsgSendAddress = 0;
        originalMsgSend = nullptr;
    }
}