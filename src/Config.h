#pragma once

#include <string_view> // For std::string_view

namespace kx {
    // Configuration for the target process and function signature
    constexpr std::string_view TARGET_PROCESS_NAME = "Gw2-64.exe";
    constexpr std::string_view MSG_SEND_PATTERN = "40 ? 48 83 EC ? 48 8D ? ? ? 48 89 ? ? 48 89 ? ? 48 89 ? ? 4C 89 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 48 8B ? E8";
}
