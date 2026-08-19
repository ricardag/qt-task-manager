#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "ProcFsPaths.h"

namespace taskmanager::datasource {

struct DiskDeviceSample {
    std::string name;
    std::uint64_t readsCompleted = 0;
    std::uint64_t sectorsRead = 0;
    std::uint64_t writesCompleted = 0;
    std::uint64_t sectorsWritten = 0;
    std::uint64_t ioTimeMs = 0; // cumulative milliseconds this device spent with I/O in flight
};

struct FilesystemUsage {
    std::string mountPoint;
    std::uint64_t totalBytes = 0;
    std::uint64_t freeBytes = 0;

    std::uint64_t usedBytes() const {
        return totalBytes > freeBytes ? totalBytes - freeBytes : 0;
    }
};

class DiskStats {
public:
    explicit DiskStats(std::filesystem::path procRoot = std::filesystem::path(kDefaultProcRoot));

    // Cumulative per-device I/O counters from /proc/diskstats. Sector size
    // is the traditional 512-byte kernel sector, per the diskstats ABI.
    std::vector<DiskDeviceSample> sampleDevices() const;

    // Space usage for a single mount point via statvfs(). Not covered by
    // the injectable procRoot: it queries the real filesystem, so it isn't
    // exercised by the deterministic unit tests.
    static FilesystemUsage usageFor(const std::string& mountPoint);

private:
    std::filesystem::path procRoot_;
};

} // namespace taskmanager::datasource
