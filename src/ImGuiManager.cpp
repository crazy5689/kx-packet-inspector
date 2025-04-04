#include "ImGuiManager.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "PacketData.h" // Include for PacketInfo, g_packetLog, g_packetLogMutex
#include "AppState.h"   // Include for UI state, filter state, hook status
#include "GuiStyle.h"  // Include for custom styling functions
#include <vector>
#include <mutex>
#include <deque>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>   // For formatting time
#include <string>
#include <map>     // For std::map used in filtering
#include "PacketHeaders.h" // Need this for iterating known headers
#include <windows.h> // Required for ShellExecuteA

bool ImGuiManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

    // Apply custom style and font
    GUIStyle::LoadAppFont(); // Load font first
    GUIStyle::ApplyCustomStyle(); // Then apply style

    if (!ImGui_ImplWin32_Init(hwnd)) return false;
    if (!ImGui_ImplDX11_Init(device, context)) return false;

    // Initialize the filter selection map (do this once)
    // Ideally, PacketHeaders.h would provide an iterable list/map,
    // but we can manually list them based on the enum for now.
    // Or better, iterate the map used in GetPacketName if accessible.
    // Let's assume GetPacketName's map isn't directly accessible and list manually.
    // This is brittle and should be improved if PacketHeaders changes often.
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_CHAT_SEND_MESSAGE] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_USE_SKILL] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_MOVEMENT] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_MOVEMENT_WITH_ROTATION] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_MOVEMENT_END] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_JUMP] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_HEARTBEAT] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_SELECT_AGENT] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_DESELECT_AGENT] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::CMSG_MOUNT_MOVEMENT] = false;
    kx::g_packetFilterSelection[kx::PacketHeader::UNKNOWN] = false; // Add UNKNOWN too

    return true;
}

void ImGuiManager::NewFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render(ID3D11DeviceContext* context, ID3D11RenderTargetView* mainRenderTargetView) {
    ImGui::EndFrame();
    ImGui::Render();
    context->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// Helper function to format timestamp
std::string FormatTimestamp(const std::chrono::system_clock::time_point& tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm local_tm;
    localtime_s(&local_tm, &time); // Use localtime_s for safety

    std::stringstream ss;
    ss << std::put_time(&local_tm, "%H:%M:%S"); // Format as HH:MM:SS
    // Add milliseconds if needed
    // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
    // ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// Helper function to format byte data as hex
std::string FormatBytesToHex(const std::vector<uint8_t>& data, int maxBytes = 32) {
    std::stringstream ss;
    int count = 0;
    for (const auto& byte : data) {
        if (count >= maxBytes) {
            ss << "...";
            break;
        }
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        count++;
    }
    return ss.str();
}


// --- Helper Functions for Rendering UI Sections ---

void ImGuiManager::RenderHints() {
    // Hints moved to top
    ImGui::TextDisabled("Hint: Press INSERT to show/hide window.");
    ImGui::TextDisabled("Hint: Press DELETE to unload DLL.");
    ImGui::Separator();
}

void ImGuiManager::RenderInfoSection() {
    // --- Info Section ---
    if (ImGui::CollapsingHeader("Info")) {
        ImGui::Text("KX Packet Inspector by Krixx");
        ImGui::Text("Visit kxtools.xyz for more tools!");
        ImGui::Separator();

        // GitHub Link
        ImGui::Text("GitHub:");
        ImGui::SameLine();
        if (ImGui::Button("Repository")) {
            ShellExecuteA(NULL, "open", "https://github.com/Krixx1337/kx-packet-inspector", NULL, NULL, SW_SHOWNORMAL);
        }

        // kxtools.xyz Link
        ImGui::Text("Website:");
        ImGui::SameLine();
        if (ImGui::Button("kxtools.xyz")) {
             ShellExecuteA(NULL, "open", "https://kxtools.xyz", NULL, NULL, SW_SHOWNORMAL);
        }

        // Discord Link
        ImGui::Text("Discord:");
        ImGui::SameLine();
        if (ImGui::Button("Join Server")) {
             ShellExecuteA(NULL, "open", "https://discord.gg/z92rnB4kHm", NULL, NULL, SW_SHOWNORMAL);
        }
        ImGui::Spacing();
    }
}

void ImGuiManager::RenderStatusControlsSection() {
    // --- Status & Controls Section ---
    if (ImGui::CollapsingHeader("Status & Controls")) {
        // Status content
        const char* presentStatusStr = (kx::g_presentHookStatus == kx::HookStatus::OK) ? "OK" : "Failed";
        ImGui::Text("Present Hook: %s", presentStatusStr);

        const char* msgSendStatusStr;
        switch (kx::g_msgSendHookStatus) {
            case kx::HookStatus::OK:       msgSendStatusStr = "OK"; break;
            case kx::HookStatus::Failed:   msgSendStatusStr = "Failed"; break;
            case kx::HookStatus::Unknown:
            default:                       msgSendStatusStr = "Not Found/Hooked"; break;
        }
        ImGui::Text("MsgSend Hook: %s", msgSendStatusStr);

        if (kx::g_msgSendAddress != 0) {
            ImGui::Text("MsgSend Address: 0x%p", (void*)kx::g_msgSendAddress);
        } else {
            ImGui::Text("MsgSend Address: N/A");
        }

        // Placeholders for Recv Hook
        const char* msgRecvStatusStr;
         switch (kx::g_msgRecvHookStatus) {
            case kx::HookStatus::OK:       msgRecvStatusStr = "OK"; break;
            case kx::HookStatus::Failed:   msgRecvStatusStr = "Failed"; break;
            case kx::HookStatus::Unknown:
            default:                       msgRecvStatusStr = "Not Found/Hooked"; break;
        }
        ImGui::Text("MsgRecv Hook: %s", msgRecvStatusStr); // Placeholder display
        if (kx::g_msgRecvAddress != 0) {
            ImGui::Text("MsgRecv Address: 0x%p", (void*)kx::g_msgRecvAddress); // Placeholder display
        } else {
            ImGui::Text("MsgRecv Address: N/A"); // Placeholder display
        }

        ImGui::Separator();

        // Controls content
        if (ImGui::Button("Clear Log")) {
            std::lock_guard<std::mutex> lock(kx::g_packetLogMutex);
            kx::g_packetLog.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Pause Capture", &kx::g_capturePaused);
        ImGui::Spacing();
    }
}

void ImGuiManager::RenderFilteringSection() {
    // --- Filtering Section ---
    if (ImGui::CollapsingHeader("Filtering")) {
        ImGui::RadioButton("Show All", reinterpret_cast<int*>(&kx::g_packetFilterMode), static_cast<int>(kx::FilterMode::ShowAll)); ImGui::SameLine();
        ImGui::RadioButton("Include Only", reinterpret_cast<int*>(&kx::g_packetFilterMode), static_cast<int>(kx::FilterMode::IncludeOnly)); ImGui::SameLine();
        ImGui::RadioButton("Exclude", reinterpret_cast<int*>(&kx::g_packetFilterMode), static_cast<int>(kx::FilterMode::Exclude));

        if (kx::g_packetFilterMode != kx::FilterMode::ShowAll) {
            // Wrap checkboxes in a scrolling child window
            ImGui::BeginChild("FilterCheckboxRegion", ImVec2(0, 150.0f), true); // Width=auto, Height=150px, Border=true
            ImGui::Indent();
            // Dynamically create checkboxes for known packet types
            // This relies on the manual initialization in Initialize() matching PacketHeaders.h
            // A better approach would involve iterating a static map/list from PacketHeaders.h
            for (auto const& [header, selected] : kx::g_packetFilterSelection) {
                 std::string name;
                 if (header == kx::PacketHeader::UNKNOWN) {
                     name = "Unknown Packets"; // Special label for the UNKNOWN category
                 } else {
                     // Cast the PacketHeader enum back to uint8_t for the GetPacketName call for known headers
                     name = kx::GetPacketName(static_cast<uint8_t>(header));
                 }
                 bool isSelected = selected; // Copy to local variable for Checkbox
                 if (ImGui::Checkbox(name.c_str(), &isSelected)) {
                     kx::g_packetFilterSelection[header] = isSelected; // Update map on change
                 }
                 // Optional: Add tooltips or columns for better layout if many headers
            }
            ImGui::Unindent();
            ImGui::EndChild();
        }
        ImGui::Spacing();
    }
    ImGui::Spacing();
}

void ImGuiManager::RenderPacketLogSection() {
    // --- Packet Log Section ---
    ImGui::Text("Packet Log:");
    ImGui::Separator();
    ImGui::BeginChild("PacketLogScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar); // Enable horizontal scrollbar

    // Pre-filter indices based on current filter settings
    std::vector<int> filtered_indices;
    { // Scope for initial lock to build filtered list
        std::lock_guard<std::mutex> lock(kx::g_packetLogMutex);
        filtered_indices.reserve(kx::g_packetLog.size()); // Optimization

        for (int i = 0; i < kx::g_packetLog.size(); ++i) {
            const auto& packet = kx::g_packetLog[i]; // Need to access packet here for filtering
            bool showPacket = true;

            if (kx::g_packetFilterMode == kx::FilterMode::IncludeOnly) {
                auto it = kx::g_packetFilterSelection.find(packet.header);
                // Show only if the header exists in the map and is checked
                if (it == kx::g_packetFilterSelection.end() || !it->second) {
                    showPacket = false;
                }
            } else if (kx::g_packetFilterMode == kx::FilterMode::Exclude) {
                auto it = kx::g_packetFilterSelection.find(packet.header);
                // Hide if the header exists in the map and is checked
                if (it != kx::g_packetFilterSelection.end() && it->second) {
                    showPacket = false;
                }
            }
            // Always show if mode is ShowAll

            if (showPacket) {
                filtered_indices.push_back(i);
            }
        }
    } // Initial lock goes out of scope

    // Now use the clipper on the filtered indices
    ImGuiListClipper clipper;
    clipper.Begin(filtered_indices.size());
    while (clipper.Step()) {
        // Lock again for the duration of rendering this clipped range
        // This is necessary because g_packetLog could potentially be modified
        // by the capture thread between the filtering step and this rendering step.
        std::lock_guard<std::mutex> lock(kx::g_packetLogMutex);

        for (int j = clipper.DisplayStart; j < clipper.DisplayEnd; ++j) {
            // Get the original index from the filtered list
            if (j < 0 || j >= filtered_indices.size()) continue; // Basic bounds check
            int original_index = filtered_indices[j];

            // Ensure the original index is still valid (in case log was cleared/shrunk)
            if (original_index >= kx::g_packetLog.size()) continue;

            const auto& packet = kx::g_packetLog[original_index];

            // Render the packet log entry
            std::string timestampStr = FormatTimestamp(packet.timestamp);
            std::string dataHexStr = FormatBytesToHex(packet.data);
            const char* directionStr = (packet.direction == kx::PacketDirection::Sent) ? "[Sent]" : "[Recv]";
            std::string logEntry = timestampStr + " | " + directionStr + " | " + packet.name + " | Size: " + std::to_string(packet.size) + " | Data: " + dataHexStr;

            ImGui::PushID(original_index); // Use original index for a stable ID
            ImGui::PushItemWidth(-FLT_MIN);
            ImGui::InputText("##PacketLogEntry", (char*)logEntry.c_str(), logEntry.size() + 1, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
    } // Rendering lock goes out of scope
    clipper.End();

    // Auto-scroll logic remains the same
    // Check scroll only if there are items to potentially scroll to
    if (!filtered_indices.empty() && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}


// --- Main Window Rendering Function ---

void ImGuiManager::RenderPacketInspectorWindow() {
    // Pass the address of the boolean to ImGui::Begin.
    // ImGui will set this to false when the user clicks the 'x' button.
    if (!kx::g_isInspectorWindowOpen) {
        // Early out if the window should be closed - prevents rendering contents after close request
        return;
    }
    // Set minimum window size constraints before calling Begin
    ImGui::SetNextWindowSizeConstraints(ImVec2(350.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX)); // Use 0.0f for no min height constraint
    ImGui::Begin("KX Packet Inspector", &kx::g_isInspectorWindowOpen);

    RenderHints();
    RenderInfoSection();
    RenderStatusControlsSection();
    RenderFilteringSection();
    RenderPacketLogSection();

    ImGui::End();
}


void ImGuiManager::RenderUI()
{
    // Only render the inspector window if the visibility flag is set
    if (kx::g_showInspectorWindow) {
        RenderPacketInspectorWindow();
    }

    // You can add other UI elements here if needed
    // ImGui::ShowDemoWindow(); // Keep demo window for reference if needed during dev
}

void ImGuiManager::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
