#pragma once

#include <string_view> // For std::string_view

namespace kx {
    // Configuration for the target process and function signature
    constexpr std::string_view TARGET_PROCESS_NAME = "Gw2-64.exe";
    constexpr std::string_view MSG_SEND_PATTERN = "40 ? 48 83 EC ? 48 8D ? ? ? 48 89 ? ? 48 89 ? ? 48 89 ? ? 4C 89 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 48 8B ? E8";

    constexpr std::string_view MSG_RECV_PATTERN = "40 55 41 54 41 55 41 56 41 57 48 83 EC ? 48 8D 6C 24 ? 48 89 5D ? 48 89 75 ? 48 89 7D ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 45 ? 44 0F B6 12";



    // --- Pattern for MsgRecv function ---
//Address of signature = Gw2 - 64.exe + 0x012EA0C0
//"\x40\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x00\x48\x8D\x6C\x24\x00\x48\x89\x5D\x00\x48\x89\x75\x00\x48\x89\x7D\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x45\x00\x44\x0F\xB6\x12", "xxxxxxxxxxxxx?xxxx?xxx?xxx?xxx?xxx????xxxxxx?xxxx"
//"40 55 41 54 41 55 41 56 41 57 48 83 EC ? 48 8D 6C 24 ? 48 89 5D ? 48 89 75 ? 48 89 7D ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 45 ? 44 0F B6 12"
}
