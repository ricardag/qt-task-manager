#pragma once

#include <cstdint>
#include <string>

namespace taskmanager::util {

// Formats a byte count as a human-readable string using binary (1024)
// prefixes, e.g. formatBytes(1536) == "1.5 KB", matching Task Manager style.
std::string formatBytes(std::uint64_t bytes);

// Formats a duration in seconds as "D:HH:MM:SS" (days omitted when zero).
std::string formatDurationSeconds(std::uint64_t seconds);

// Formats a 0-100 ratio as "N%", clamping out-of-range input.
std::string formatPercent(double percent);

} // namespace taskmanager::util
