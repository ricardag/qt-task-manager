#include "DiskStats.h"

#include <charconv>

#include <sys/statvfs.h>

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

DiskStats::DiskStats(std::filesystem::path procRoot) : procRoot_(std::move(procRoot)) {}

std::vector<DiskDeviceSample> DiskStats::sampleDevices() const {
    std::vector<DiskDeviceSample> result;

    const auto content = readFile(procRoot_ / "diskstats");
    if (!content) return result;

    for (const std::string& line : splitLines(*content)) {
        const std::vector<std::string> fields = splitWhitespace(line);
        // major minor name reads_completed reads_merged sectors_read time_reading
        // writes_completed writes_merged sectors_written time_writing
        // ios_in_progress time_ios weighted_time_ios ...
        if (fields.size() < 10) continue;

        DiskDeviceSample sample;
        sample.name = fields[2];
        sample.readsCompleted = toU64(fields[3]);
        sample.sectorsRead = toU64(fields[5]);
        sample.writesCompleted = toU64(fields[7]);
        sample.sectorsWritten = toU64(fields[9]);
        if (fields.size() > 12) sample.ioTimeMs = toU64(fields[12]);
        result.push_back(std::move(sample));
    }

    return result;
}

FilesystemUsage DiskStats::usageFor(const std::string& mountPoint) {
    FilesystemUsage usage;
    usage.mountPoint = mountPoint;

    struct statvfs vfs {};
    if (statvfs(mountPoint.c_str(), &vfs) != 0) {
        return usage;
    }

    usage.totalBytes = static_cast<std::uint64_t>(vfs.f_blocks) * vfs.f_frsize;
    usage.freeBytes = static_cast<std::uint64_t>(vfs.f_bavail) * vfs.f_frsize;
    return usage;
}

} // namespace taskmanager::datasource
