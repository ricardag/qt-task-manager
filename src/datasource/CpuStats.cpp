#include "CpuStats.h"

#include <charconv>

#include "../util/ProcFileUtils.h"

namespace taskmanager::datasource {

using taskmanager::util::readFile;
using taskmanager::util::splitLines;
using taskmanager::util::splitWhitespace;

namespace {

std::uint64_t toU64(const std::string& s) {
    std::uint64_t value = 0;
    std::from_chars(s.data(), s.data() + s.size(), value);
    return value;
}

} // namespace

CpuStats::CpuStats(std::filesystem::path procRoot) : procRoot_(std::move(procRoot)) {}

std::vector<CpuCoreTicks> CpuStats::sampleTicks() const {
    std::vector<CpuCoreTicks> samples;

    const auto content = readFile(procRoot_ / "stat");
    if (!content) {
        return samples;
    }

    for (const std::string& line : splitLines(*content)) {
        if (line.rfind("cpu", 0) != 0) {
            continue; // stop at non-cpu lines like "intr", "ctxt", ...
        }
        const std::vector<std::string> fields = splitWhitespace(line);
        if (fields.size() < 8) continue;

        CpuCoreTicks ticks;
        ticks.label = fields[0];
        ticks.user = toU64(fields[1]);
        ticks.nice = toU64(fields[2]);
        ticks.system = toU64(fields[3]);
        ticks.idle = toU64(fields[4]);
        ticks.iowait = toU64(fields[5]);
        ticks.irq = toU64(fields[6]);
        ticks.softirq = toU64(fields[7]);
        if (fields.size() > 8) ticks.steal = toU64(fields[8]);
        samples.push_back(std::move(ticks));
    }

    return samples;
}

int CpuStats::logicalCoreCount() const {
    const auto content = readFile(procRoot_ / "cpuinfo");
    if (!content) return 0;

    int count = 0;
    for (const std::string& line : splitLines(*content)) {
        if (line.rfind("processor", 0) == 0) {
            ++count;
        }
    }
    return count;
}

std::string CpuStats::modelName() const {
    const auto content = readFile(procRoot_ / "cpuinfo");
    if (!content) return {};

    for (const std::string& line : splitLines(*content)) {
        if (line.rfind("model name", 0) == 0) {
            const auto colon = line.find(':');
            if (colon == std::string::npos) continue;
            std::string value = line.substr(colon + 1);
            while (!value.empty() && value.front() == ' ') value.erase(value.begin());
            return value;
        }
    }
    return {};
}

double CpuStats::usagePercent(const CpuCoreTicks& previous, const CpuCoreTicks& current) {
    const std::uint64_t totalDelta = current.total() - previous.total();
    if (totalDelta == 0) return 0.0;

    const std::uint64_t idleDelta = current.idleTotal() - previous.idleTotal();
    const std::uint64_t busyDelta = totalDelta - idleDelta;
    return (static_cast<double>(busyDelta) / static_cast<double>(totalDelta)) * 100.0;
}

} // namespace taskmanager::datasource
