#include "Formatting.h"

#include <algorithm>
#include <array>
#include <cstdio>

namespace taskmanager::util {

std::string formatBytes(std::uint64_t bytes) {
    static constexpr std::array<const char*, 6> units = {"B", "KB", "MB", "GB", "TB", "PB"};
    double value = static_cast<double>(bytes);
    std::size_t unitIndex = 0;
    while (value >= 1024.0 && unitIndex + 1 < units.size()) {
        value /= 1024.0;
        ++unitIndex;
    }
    char buffer[32];
    if (unitIndex == 0) {
        std::snprintf(buffer, sizeof(buffer), "%llu %s", static_cast<unsigned long long>(bytes), units[unitIndex]);
    } else {
        std::snprintf(buffer, sizeof(buffer), "%.1f %s", value, units[unitIndex]);
    }
    return std::string(buffer);
}

std::string formatDurationSeconds(std::uint64_t seconds) {
    const std::uint64_t days = seconds / 86400;
    const std::uint64_t hours = (seconds % 86400) / 3600;
    const std::uint64_t minutes = (seconds % 3600) / 60;
    const std::uint64_t secs = seconds % 60;

    char buffer[32];
    if (days > 0) {
        std::snprintf(buffer, sizeof(buffer), "%llu:%02llu:%02llu:%02llu",
                      static_cast<unsigned long long>(days), static_cast<unsigned long long>(hours),
                      static_cast<unsigned long long>(minutes), static_cast<unsigned long long>(secs));
    } else {
        std::snprintf(buffer, sizeof(buffer), "%02llu:%02llu:%02llu",
                      static_cast<unsigned long long>(hours), static_cast<unsigned long long>(minutes),
                      static_cast<unsigned long long>(secs));
    }
    return std::string(buffer);
}

std::string formatPercent(double percent) {
    const double clamped = std::clamp(percent, 0.0, 100.0);
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%.0f%%", clamped);
    return std::string(buffer);
}

} // namespace taskmanager::util
