#include "MemoryStats.h"

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

MemoryStats::MemoryStats(std::filesystem::path procRoot) : procRoot_(std::move(procRoot)) {}

MemorySample MemoryStats::sample() const {
    MemorySample result;

    const auto content = readFile(procRoot_ / "meminfo");
    if (!content) return result;

    for (const std::string& line : splitLines(*content)) {
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        const std::string key = line.substr(0, colon);
        const std::vector<std::string> valueTokens = splitWhitespace(line.substr(colon + 1));
        if (valueTokens.empty()) continue;
        const std::uint64_t value = toU64(valueTokens[0]);

        if (key == "MemTotal") result.totalKb = value;
        else if (key == "MemFree") result.freeKb = value;
        else if (key == "MemAvailable") result.availableKb = value;
        else if (key == "Buffers") result.buffersKb = value;
        else if (key == "Cached") result.cachedKb = value;
        else if (key == "SwapTotal") result.swapTotalKb = value;
        else if (key == "SwapFree") result.swapFreeKb = value;
    }

    return result;
}

} // namespace taskmanager::datasource
