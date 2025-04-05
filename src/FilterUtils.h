#pragma once

#include "PacketData.h" // For PacketInfo, PacketDirection
#include "AppState.h"   // For filter modes and selections
#include <vector>
#include <deque>

namespace kx::Filtering {

    /**
     * @brief Applies the current global filters to the packet log.
     * @param fullLog A const reference to the complete packet log deque.
     * @return A vector containing the indices (relative to fullLog) of packets that pass the filters.
     */
    std::vector<int> GetFilteredPacketIndices(const std::deque<kx::PacketInfo>& fullLog);

    /**
     * @brief Checks if a single packet passes the current global filters.
     * @param packet The packet to check.
     * @return True if the packet should be displayed, false otherwise.
     */
    bool ShouldDisplayPacket(const kx::PacketInfo& packet);

} // namespace kx::Filtering