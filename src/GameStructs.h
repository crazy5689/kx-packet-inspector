#pragma once

#include <cstddef> // For std::byte, std::size_t
#include <cstdint> // For std::uint8_t, std::int32_t
#include <type_traits> // For offsetof (used in static_assert)

namespace kx::GameStructs {

    /**
     * @brief Represents the memory layout of the context object passed to the
     *        game's internal message sending function (identified as FUN_1412e9960).
     * @details This structure defines members based on reverse engineering the function,
     *          specifically how it accesses data relative to its 'param_1' argument.
     *          It allows accessing packet data before potential encryption/final processing.
     * @warning This structure definition is based on analysis of specific versions
     *          of the target game (Gw2-64.exe). While it may remain stable across
     *          some game updates, patches that significantly alter internal data
     *          layouts **could** require this definition to be updated to ensure
     *          correct functionality. Community contributions to verify and update
     *          this definition are appreciated.
     */
    struct MsgSendContext {
        // --- Member offsets relative to the start of the structure (param_1) ---

        // Define padding explicitly for clarity on known/unknown regions.
        std::byte padding_0x0[0xD0];

        /**
         * @brief 0xD0: Pointer to the current write position (end) within the packet data buffer.
         * @details This pointer indicates how much data has been written to the buffer located
         *          at PACKET_BUFFER_OFFSET relative to 'this'. The size of the current packet
         *          data is calculated as (currentBufferEndPtr - GetPacketBufferStart()).
         *          The function modifies this pointer, notably resetting it when bufferState == 1
         *          or after processing the buffer in the encryption path.
         */
        std::uint8_t* currentBufferEndPtr; // Corresponds to *(param_1 + 0xd0)

        // Padding between known members.
        std::byte padding_0xD8[0x108 - 0xD8];

        /**
         * @brief 0x108: State variable influencing control flow within the send function.
         * @details Observed values in decompilation (e.g., 1, 3) determine the processing path.
         *          State '1' appears to skip the main processing/encryption and resets
         *          currentBufferEndPtr, suggesting the buffer should not be read in this state.
         *          State '3' is associated with the path that includes encryption calls.
         *          (See the 'if (*(int *)(param_1 + 0x108) == 1)' branch in FUN_1412e9960).
         */
        std::int32_t bufferState; // Corresponds to *(param_1 + 0x108)

        // --- Constants Related to the Context ---

        /**
         * @brief The fixed memory offset from the base address of the MsgSendContext instance
         *        to the start of the actual raw packet data buffer.
         * @details This buffer itself is not directly part of the C++ struct layout.
         */
        static constexpr std::size_t PACKET_BUFFER_OFFSET = 0x398;

        // --- Helper Methods ---

        /**
         * @brief Calculates the pointer to the start of the raw packet data buffer.
         * @return Pointer to the beginning of the packet data.
         */
        std::uint8_t* GetPacketBufferStart() const noexcept {
            // Use C-style cast for pointer arithmetic from 'this'.
            return reinterpret_cast<std::uint8_t*>(
                const_cast<MsgSendContext*>(this)
                ) + PACKET_BUFFER_OFFSET;
        }

        /**
         * @brief Calculates the size of the packet data currently present in the buffer.
         * @return The size in bytes, or 0 if pointers are invalid or end < start.
         */
        std::size_t GetCurrentDataSize() const noexcept {
            const std::uint8_t* bufferStart = GetPacketBufferStart();
            // Basic validation: ensure end pointer is valid and after start pointer.
            if (currentBufferEndPtr == nullptr || currentBufferEndPtr < bufferStart) {
                return 0;
            }
            return static_cast<std::size_t>(currentBufferEndPtr - bufferStart);
        }
    };

    // Static assertions to verify expected offsets at compile time.
    // Helps catch errors if the struct definition or compiler padding changes unexpectedly.
    static_assert(offsetof(MsgSendContext, currentBufferEndPtr) == 0xD0, "Offset mismatch for MsgSendContext::currentBufferEndPtr");
    static_assert(offsetof(MsgSendContext, bufferState) == 0x108, "Offset mismatch for MsgSendContext::bufferState");

}