#pragma once

#include <cstdint>
#include <string>

namespace taskmanager::datasource {

// Raw per-process snapshot as read from /proc/[pid]/{stat,status,cmdline,io}
// at a single point in time. CPU-usage percentage is NOT computed here: it
// requires the delta between two samples, which SystemMonitor derives.
struct ProcessInfo {
    int pid = 0;
    int ppid = 0;
    std::string name;      // "comm" field from /proc/[pid]/stat
    std::string cmdline;   // joined /proc/[pid]/cmdline, empty for kernel threads
    char state = '?';      // R, S, D, Z, T, ...
    int threadCount = 0;
    int uid = -1;
    std::uint64_t vmRssKb = 0;
    std::uint64_t vmSizeKb = 0;
    std::uint64_t utimeTicks = 0; // user-mode CPU ticks, cumulative since process start
    std::uint64_t stimeTicks = 0; // kernel-mode CPU ticks, cumulative since process start
    int priority = 0;
    int niceValue = 0;
    std::uint64_t readBytes = 0;  // from /proc/[pid]/io: read_bytes
    std::uint64_t writeBytes = 0; // from /proc/[pid]/io: write_bytes
};

} // namespace taskmanager::datasource
