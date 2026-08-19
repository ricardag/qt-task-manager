#include "NetworkStats.h"

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

NetworkStats::NetworkStats(std::filesystem::path procRoot) : procRoot_(std::move(procRoot)) {}

std::vector<NetworkInterfaceSample> NetworkStats::sample() const {
    std::vector<NetworkInterfaceSample> result;

    const auto content = readFile(procRoot_ / "net" / "dev");
    if (!content) return result;

    const std::vector<std::string> lines = splitLines(*content);
    // Skip the two header lines ("Inter-|..." and " face |bytes ...").
    for (std::size_t i = 2; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string name = line.substr(0, colon);
        while (!name.empty() && name.front() == ' ') name.erase(name.begin());

        const std::vector<std::string> fields = splitWhitespace(line.substr(colon + 1));
        if (fields.size() < 10) continue;

        NetworkInterfaceSample sample;
        sample.name = std::move(name);
        sample.rxBytes = toU64(fields[0]);
        sample.rxPackets = toU64(fields[1]);
        sample.txBytes = toU64(fields[8]);
        sample.txPackets = toU64(fields[9]);
        result.push_back(std::move(sample));
    }

    return result;
}

} // namespace taskmanager::datasource
