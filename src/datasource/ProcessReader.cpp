#include "ProcessReader.h"

#include <algorithm>
#include <charconv>
#include <cstdlib>

#include "ProcFsPaths.h"
#include "../util/ProcFileUtils.h"

namespace taskmanager::datasource {

using taskmanager::util::readFile;
using taskmanager::util::splitLines;
using taskmanager::util::splitWhitespace;

namespace {

std::optional<long long> toLongLong(const std::string& s) {
    long long value = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec != std::errc() || ptr != s.data() + s.size()) {
        return std::nullopt;
    }
    return value;
}

// Parses "pid (comm) state ppid ... utime stime ... priority nice
// num_threads ..." from /proc/[pid]/stat. The comm field is delimited by
// the *outermost* parentheses because process names can themselves contain
// parentheses or spaces (e.g. a script renamed to "(evil) worker").
bool parseStat(const std::string& content, ProcessInfo& info) {
    const auto openParen = content.find('(');
    const auto closeParen = content.rfind(')');
    if (openParen == std::string::npos || closeParen == std::string::npos || closeParen < openParen) {
        return false;
    }

    info.name = content.substr(openParen + 1, closeParen - openParen - 1);

    const std::string rest = content.substr(closeParen + 1);
    const std::vector<std::string> fields = splitWhitespace(rest);
    // fields[0]=state 1=ppid ... 11=utime 12=stime ... 15=priority 16=nice 17=num_threads
    if (fields.size() < 18) {
        return false;
    }

    info.state = fields[0].empty() ? '?' : fields[0][0];
    if (auto v = toLongLong(fields[1])) info.ppid = static_cast<int>(*v);
    if (auto v = toLongLong(fields[11])) info.utimeTicks = static_cast<std::uint64_t>(*v);
    if (auto v = toLongLong(fields[12])) info.stimeTicks = static_cast<std::uint64_t>(*v);
    if (auto v = toLongLong(fields[15])) info.priority = static_cast<int>(*v);
    if (auto v = toLongLong(fields[16])) info.niceValue = static_cast<int>(*v);
    if (auto v = toLongLong(fields[17])) info.threadCount = static_cast<int>(*v);
    return true;
}

void parseStatus(const std::string& content, ProcessInfo& info) {
    for (const std::string& line : splitLines(content)) {
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        const std::string key = line.substr(0, colon);
        const std::vector<std::string> valueTokens = splitWhitespace(line.substr(colon + 1));
        if (valueTokens.empty()) continue;

        if (key == "VmRSS") {
            if (auto v = toLongLong(valueTokens[0])) info.vmRssKb = static_cast<std::uint64_t>(*v);
        } else if (key == "VmSize") {
            if (auto v = toLongLong(valueTokens[0])) info.vmSizeKb = static_cast<std::uint64_t>(*v);
        } else if (key == "Threads") {
            if (auto v = toLongLong(valueTokens[0])) info.threadCount = static_cast<int>(*v);
        } else if (key == "Uid") {
            if (auto v = toLongLong(valueTokens[0])) info.uid = static_cast<int>(*v);
        }
    }
}

std::string parseCmdline(const std::string& content) {
    std::string result = content;
    std::replace(result.begin(), result.end(), '\0', ' ');
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}

void parseIo(const std::string& content, ProcessInfo& info) {
    for (const std::string& line : splitLines(content)) {
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        const std::string key = line.substr(0, colon);
        const std::vector<std::string> valueTokens = splitWhitespace(line.substr(colon + 1));
        if (valueTokens.empty()) continue;

        if (key == "read_bytes") {
            if (auto v = toLongLong(valueTokens[0])) info.readBytes = static_cast<std::uint64_t>(*v);
        } else if (key == "write_bytes") {
            if (auto v = toLongLong(valueTokens[0])) info.writeBytes = static_cast<std::uint64_t>(*v);
        }
    }
}

bool isAllDigits(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); });
}

} // namespace

std::filesystem::path ProcessReader::kDefaultProcRootPath() {
    return std::filesystem::path(kDefaultProcRoot);
}

ProcessReader::ProcessReader(std::filesystem::path procRoot) : procRoot_(std::move(procRoot)) {}

std::optional<ProcessInfo> ProcessReader::readOne(int pid) const {
    const std::filesystem::path pidDir = procRoot_ / std::to_string(pid);

    const auto statContent = readFile(pidDir / "stat");
    if (!statContent) {
        return std::nullopt;
    }

    ProcessInfo info;
    info.pid = pid;
    if (!parseStat(*statContent, info)) {
        return std::nullopt;
    }

    if (const auto statusContent = readFile(pidDir / "status")) {
        parseStatus(*statusContent, info);
    }
    if (const auto cmdlineContent = readFile(pidDir / "cmdline")) {
        info.cmdline = parseCmdline(*cmdlineContent);
    }
    if (const auto ioContent = readFile(pidDir / "io")) {
        parseIo(*ioContent, info);
    }

    return info;
}

std::vector<ProcessInfo> ProcessReader::readAll() const {
    std::vector<ProcessInfo> result;

    std::error_code ec;
    if (!std::filesystem::exists(procRoot_, ec)) {
        return result;
    }

    for (const auto& entry : std::filesystem::directory_iterator(procRoot_, ec)) {
        if (!entry.is_directory()) continue;
        const std::string dirName = entry.path().filename().string();
        if (!isAllDigits(dirName)) continue;

        const int pid = std::atoi(dirName.c_str());
        if (auto info = readOne(pid)) {
            result.push_back(std::move(*info));
        }
    }

    return result;
}

} // namespace taskmanager::datasource
