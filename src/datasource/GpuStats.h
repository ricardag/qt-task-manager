#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "ProcFsPaths.h"

namespace taskmanager::datasource {

struct GpuSample {
    std::string cardName;   // e.g. "card0"
    int usagePercent = -1;  // -1 if unavailable
    std::uint64_t vramUsedBytes = 0;
    std::uint64_t vramTotalBytes = 0;
};

// Best-effort GPU monitoring via sysfs (amdgpu/i915 busy-percent and VRAM
// counters). There is no universal Linux API for GPU usage: AMD and Intel
// expose some counters under /sys/class/drm, NVIDIA does not without the
// proprietary driver's NVML library. NVIDIA support, if added later, must
// load NVML via dlopen() at runtime rather than being linked unconditionally,
// since it's neither installed nor relevant on non-NVIDIA machines or on
// arm64 boards without an NVIDIA GPU.
class GpuStats {
public:
    explicit GpuStats(std::filesystem::path sysRoot = std::filesystem::path(kDefaultSysRoot));

    // One entry per /sys/class/drm/cardN/device directory that exposes at
    // least a busy-percent or VRAM counter. Empty if the machine's GPU
    // driver doesn't expose any of the sysfs files this reads.
    std::vector<GpuSample> sample() const;

private:
    std::filesystem::path sysRoot_;
};

} // namespace taskmanager::datasource
