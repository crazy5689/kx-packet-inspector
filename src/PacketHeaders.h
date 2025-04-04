#pragma once

/*
 * NOTE ON SCOPE:
 * This file defines known packet headers for identification purposes.
 * The target client potentially contains around ~1700 distinct network messages,
 * falling into roughly ~35 relevant categories. This list represents only a
 * small subset identified so far. Contributions for defining more headers
 * and potentially categorizing them are welcome.
 */

#include <cstdint>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>

namespace kx {

// Enum for known Client-to-Server (CMSG) packet headers
enum class PacketHeader : uint8_t {
    CMSG_CHAT_SEND_MESSAGE      = 0xF9,
    CMSG_USE_SKILL              = 0x17,
    CMSG_MOVEMENT               = 0x12,
    CMSG_MOVEMENT_WITH_ROTATION = 0x0B,
    CMSG_MOVEMENT_END           = 0x04,
    CMSG_JUMP                   = 0x09,
    CMSG_HEARTBEAT              = 0x11,
    CMSG_SELECT_AGENT           = 0xDE,
    CMSG_DESELECT_AGENT         = 0xD6,
    CMSG_MOUNT_MOVEMENT         = 0x18,
    // Add more known headers here
    UNKNOWN                     = 0xFF // Use a value unlikely to be a real header for unknown
};

// Helper function to get a string representation of a packet header
// Map for efficient lookup (can be kept internal to GetPacketName)
namespace { // Anonymous namespace to limit scope
    const std::map<PacketHeader, std::string>& GetPacketNameMap() {
        static const std::map<PacketHeader, std::string> packetNames = {
            { PacketHeader::CMSG_CHAT_SEND_MESSAGE,      "CMSG_CHAT_SEND_MESSAGE" },
            { PacketHeader::CMSG_USE_SKILL,              "CMSG_USE_SKILL" },
            { PacketHeader::CMSG_MOVEMENT,               "CMSG_MOVEMENT" },
            { PacketHeader::CMSG_MOVEMENT_WITH_ROTATION, "CMSG_MOVEMENT_WITH_ROTATION" },
            { PacketHeader::CMSG_MOVEMENT_END,           "CMSG_MOVEMENT_END" },
            { PacketHeader::CMSG_JUMP,                   "CMSG_JUMP" },
            { PacketHeader::CMSG_HEARTBEAT,              "CMSG_HEARTBEAT" },
            { PacketHeader::CMSG_SELECT_AGENT,           "CMSG_SELECT_AGENT" },
            { PacketHeader::CMSG_DESELECT_AGENT,         "CMSG_DESELECT_AGENT" },
            { PacketHeader::CMSG_MOUNT_MOVEMENT,         "CMSG_MOUNT_MOVEMENT" }
            // Add corresponding names for other headers here
        };
        return packetNames;
    }
}

// Helper function to attempt converting a raw byte to a known PacketHeader
// Defined before GetPacketName because GetPacketName calls this.
inline PacketHeader ToPacketHeader(uint8_t rawHeader) {
     // Check if the raw value corresponds to any known enum value
     switch(rawHeader) {
        case static_cast<uint8_t>(PacketHeader::CMSG_CHAT_SEND_MESSAGE):      return PacketHeader::CMSG_CHAT_SEND_MESSAGE;
        case static_cast<uint8_t>(PacketHeader::CMSG_USE_SKILL):              return PacketHeader::CMSG_USE_SKILL;
        case static_cast<uint8_t>(PacketHeader::CMSG_MOVEMENT):               return PacketHeader::CMSG_MOVEMENT;
        case static_cast<uint8_t>(PacketHeader::CMSG_MOVEMENT_WITH_ROTATION): return PacketHeader::CMSG_MOVEMENT_WITH_ROTATION;
        case static_cast<uint8_t>(PacketHeader::CMSG_MOVEMENT_END):           return PacketHeader::CMSG_MOVEMENT_END;
        case static_cast<uint8_t>(PacketHeader::CMSG_JUMP):                   return PacketHeader::CMSG_JUMP;
        case static_cast<uint8_t>(PacketHeader::CMSG_HEARTBEAT):              return PacketHeader::CMSG_HEARTBEAT;
        case static_cast<uint8_t>(PacketHeader::CMSG_SELECT_AGENT):           return PacketHeader::CMSG_SELECT_AGENT;
        case static_cast<uint8_t>(PacketHeader::CMSG_DESELECT_AGENT):         return PacketHeader::CMSG_DESELECT_AGENT;
        case static_cast<uint8_t>(PacketHeader::CMSG_MOUNT_MOVEMENT):         return PacketHeader::CMSG_MOUNT_MOVEMENT;
        // Add cases for other known headers
        default:                                                              return PacketHeader::UNKNOWN; // Treat unknown values distinctly
     }
}

// Helper function to get a string representation of a packet header from the raw byte value
inline std::string GetPacketName(uint8_t rawHeaderValue) {
    PacketHeader headerEnum = ToPacketHeader(rawHeaderValue); // Convert to enum first

    if (headerEnum != PacketHeader::UNKNOWN) {
        // If it's a known header, look it up in the map
        const auto& packetNames = GetPacketNameMap();
        auto it = packetNames.find(headerEnum);
        if (it != packetNames.end()) {
            return it->second;
        }
        // Fallthrough to unknown formatting if map lookup fails (shouldn't happen if map is synced with enum)
    }

    // If unknown or map lookup failed, format using the raw value
    std::stringstream ss;
    ss << "Unknown [0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rawHeaderValue) << "]";
    return ss.str();
}


}