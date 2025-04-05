#include "FormattingUtils.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// Include PacketData.h again here for the implementation details of PacketInfo if needed,
// although it's already included via the header. Best practice includes what you use.
#include "PacketData.h" // Provides PacketInfo definition, PacketDirection

namespace kx::Utils {

    // --- Function Implementations ---

    std::string FormatTimestamp(const std::chrono::system_clock::time_point& tp) {
        std::time_t time = std::chrono::system_clock::to_time_t(tp);
        std::tm local_tm;
        localtime_s(&local_tm, &time); // Use localtime_s for safety

        std::stringstream ss;
        ss << std::put_time(&local_tm, "%H:%M:%S");
        // Optional: milliseconds
        // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
        // ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    std::string FormatBytesToHex(const std::vector<uint8_t>& data, int maxBytes) {
        std::stringstream ss;
        int count = 0;
        bool truncated = false; // Flag to check if truncation happened

        for (const auto& byte : data) {
            // Apply maxBytes limit (only if positive)
            if (maxBytes > 0 && count >= maxBytes) {
                ss << "...";
                truncated = true;
                break; // Stop processing bytes
            }

            if (count > 0) ss << " "; // Add space before the next byte (if not the first)

            ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            count++;
        }

        // Handle edge cases for display string
        if (data.empty()) {
            return "(empty)";
        }
        // If maxBytes forced truncation and no bytes were printed (e.g., maxBytes=0)
        if (count == 0 && maxBytes == 0 && !data.empty()) {
            return "...";
        }
        // If data exists, but limit was 0 or negative, and nothing printed (shouldn't happen with loop fix)
        if (ss.str().empty() && !data.empty() && !truncated) {
            // This case might indicate an issue, but return something sensible.
            // If maxBytes <= 0, it should have printed everything.
            return "(Error Formatting Hex?)"; // Or return empty string ""
        }


        return ss.str();
    }

    std::string FormatDisplayLogEntryString(const PacketInfo& packet, int maxHexBytes) {
        std::string timestampStr = FormatTimestamp(packet.timestamp);
        const char* directionStr = (packet.direction == PacketDirection::Sent) ? "[S]" : "[R]";
        const auto& dataToDisplay = packet.GetDisplayData(); // Use helper for potentially decrypted data
        int displaySize = dataToDisplay.size();

        // *** Use the maxHexBytes parameter for display ***
        std::string dataHexStr = FormatBytesToHex(dataToDisplay, maxHexBytes);

        std::stringstream ss;
        ss << timestampStr << " " << directionStr << " "
            << packet.name // Use the pre-resolved name
            << " | Sz:" << displaySize
            << " | " << dataHexStr;
        return ss.str();
    }

    std::string FormatFullLogEntryString(const PacketInfo& packet) {
        std::string timestampStr = FormatTimestamp(packet.timestamp);
        const char* directionStr = (packet.direction == PacketDirection::Sent) ? "[S]" : "[R]";
        const auto& dataToDisplay = packet.GetDisplayData();
        int displaySize = dataToDisplay.size();

        // *** Call FormatBytesToHex with -1 (or 0) for no limit ***
        std::string dataHexStr = FormatBytesToHex(dataToDisplay, -1); // Use -1 for unlimited

        std::stringstream ss;
        ss << timestampStr << " " << directionStr << " "
            << packet.name
            << " | Sz:" << displaySize
            << " | " << dataHexStr;
        return ss.str();
    }

} // namespace kx::Utils