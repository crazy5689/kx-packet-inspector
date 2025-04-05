#pragma once

#include "HookManager.h"
#include "D3DRenderHook.h"
#include "MsgSendHook.h"
#include "MsgRecvHook.h"

namespace kx {

    // Namespace to group game-specific hook initialization logic
    namespace GameHooks {
        /**
         * @brief Finds and initializes the MsgSend hook.
         * @return True if successful or pattern not found (non-fatal), false on hooking error.
         */
        bool InitializeMsgSendHook();

        /**
         * @brief Finds and initializes the MsgRecv hook.
         * @return True if successful or pattern not found (non-fatal), false on hooking error.
         */
        bool InitializeMsgRecvHook();

        /**
         * @brief Cleans up game-specific hooks (if needed beyond HookManager::Shutdown).
         */
        void Shutdown(); // Optional: May not be needed if cleanup is just hook removal
    } // namespace GameHooks

    /**
     * @brief Orchestrates the initialization of all required hooks.
     * @return True if essential hooks (like Present) were initialized, false otherwise.
     */
    bool InitializeHooks();

    /**
     * @brief Orchestrates the shutdown and cleanup of all hooks and related systems.
     */
    void CleanupHooks();

} // namespace kx