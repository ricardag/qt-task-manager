#pragma once

#include <cstdint>
#include <filesystem>

#include "ProcFsPaths.h"

namespace taskmanager::datasource {

struct MemorySample {
    std::uint64_t totalKb = 0;
    std::uint64_t freeKb = 0;
    std::uint64_t availableKb = 0;
    std::uint64_t buffersKb = 0;
    std::uint64_t cachedKb = 0;
    std::uint64_t swapTotalKb = 0;
    std::uint64_t swapFreeKb = 0;

    std::uint64_t usedKb() const {
        return totalKb > availableKb ? totalKb - availableKb : 0;
    }
    std::uint64_t swapUsedKb() const {
        return swapTotalKb > swapFreeKb ? swapTotalKb - swapFreeKb : 0;
    }
};

class MemoryStats {
public:
    explicit MemoryStats(std::filesystem::path procRoot = std::filesystem::path(kDefaultProcRoot));

    MemorySample sample() const;

private:
    std::filesystem::path procRoot_;
};

} // namespace taskmanager::datasource
