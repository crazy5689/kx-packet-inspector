#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint> // For uint8_t

// Forward declare PacketInfo to avoid including PacketData.h in the header if possible,
// but since the function signature requires it, we must include it.
#include "PacketData.h" // Include necessary dependencies for function signatures

namespace kx::Utils {

    /**
     * @brief Formats a system time point into HH:MM:SS string.
     * @param tp The time point to format.
     * @return Formatted time string.
     */
    std::string FormatTimestamp(const std::chrono::system_clock::time_point& tp);

    /**
     * @brief Formats a vector of bytes into a space-separated hex string.
     * @param data The byte vector.
     * @param maxBytes Max bytes before truncating with "...". <= 0 means no limit.
     * @return Formatted hex string.
     */
    std::string FormatBytesToHex(const std::vector<uint8_t>& data, int maxBytes = 32);

    /**
     * @brief Formats a PacketInfo for display (potentially truncated hex).
     * @param packet The PacketInfo object.
     * @param maxHexBytes Max hex bytes to display before adding "...".
     * @return A formatted string for the log display.
     */
    std::string FormatDisplayLogEntryString(const PacketInfo& packet, int maxHexBytes = 32);

    /**
     * @brief Formats a PacketInfo for copying (full, untruncated hex).
     * @param packet The PacketInfo object.
     * @return A formatted string with the complete hex data.
     */
    std::string FormatFullLogEntryString(const PacketInfo& packet);

} // namespace kx::Utils