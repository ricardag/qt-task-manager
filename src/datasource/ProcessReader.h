#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "ProcessInfo.h"

namespace taskmanager::datasource {

// Reads the process table from a /proc-like directory tree. The root is
// injectable so tests can point it at tests/fixtures/proc instead of the
// real /proc, making results deterministic.
class ProcessReader {
public:
    explicit ProcessReader(std::filesystem::path procRoot = kDefaultProcRootPath());

    // Lists all currently readable processes. Processes that vanish between
    // being listed and being read are silently skipped, not reported as
    // errors.
    std::vector<ProcessInfo> readAll() const;

    // Reads a single process by PID. Returns nullopt if it no longer exists
    // or its /proc entry can't be parsed.
    std::optional<ProcessInfo> readOne(int pid) const;

private:
    static std::filesystem::path kDefaultProcRootPath();

    std::filesystem::path procRoot_;
};

} // namespace taskmanager::datasource
