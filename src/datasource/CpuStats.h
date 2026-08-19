#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "ProcFsPaths.h"

namespace taskmanager::datasource {

// Raw cumulative tick counters for one CPU (aggregate "cpu" line or a
// single "cpuN" line) from /proc/stat. Values are monotonically increasing
// since boot; usage percentage requires the delta between two samples.
struct CpuCoreTicks {
    std::string label; // "cpu" for the aggregate, "cpu0", "cpu1", ... per core
    std::uint64_t user = 0;
    std::uint64_t nice = 0;
    std::uint64_t system = 0;
    std::uint64_t idle = 0;
    std::uint64_t iowait = 0;
    std::uint64_t irq = 0;
    std::uint64_t softirq = 0;
    std::uint64_t steal = 0;

    std::uint64_t total() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
    std::uint64_t idleTotal() const { return idle + iowait; }
};

class CpuStats {
public:
    explicit CpuStats(std::filesystem::path procRoot = std::filesystem::path(kDefaultProcRoot));

    // First element is the aggregate "cpu" line, followed by one entry per
    // logical core ("cpu0", "cpu1", ...), in /proc/stat order.
    std::vector<CpuCoreTicks> sampleTicks() const;

    // Number of logical cores, from /proc/cpuinfo "processor" lines.
    int logicalCoreCount() const;

    // CPU model name, from the first "model name" line in /proc/cpuinfo.
    std::string modelName() const;

    // Usage percentage between two samples of the same core, using the
    // standard (busy delta / total delta) * 100 formula. Returns 0 if the
    // total delta is zero (no time elapsed, or counters reset).
    static double usagePercent(const CpuCoreTicks& previous, const CpuCoreTicks& current);

private:
    std::filesystem::path procRoot_;
};

} // namespace taskmanager::datasource
