#pragma once

#include <vector>
#include <string> // Needed for std::string name
#include "PacketHeaders.h"
#include <mutex>
#include <deque>
#include <chrono>
#include <cstdint> // For uint8_t

namespace kx {

// Enum to represent packet direction
enum class PacketDirection {
    Sent,
    Received
};

// Structure to hold information about a captured packet
struct PacketInfo {
    std::chrono::system_clock::time_point timestamp;
    int size;
    std::vector<uint8_t> data; // Store the actual byte data
    PacketDirection direction;
    PacketHeader    header; // Identified header enum
    std::string     name;   // String name of the header
};

// Global container for storing captured packet info
// Using deque for efficient push_front/pop_back if needed, or just push_back
extern std::deque<PacketInfo> g_packetLog;

// Mutex to protect access to the global packet log
extern std::mutex g_packetLogMutex;



}