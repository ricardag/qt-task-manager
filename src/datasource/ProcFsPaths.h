#pragma once

namespace taskmanager::datasource {

// Default roots for the pseudo-filesystems this layer reads from.
// Tests override these with paths under tests/fixtures/proc to get
// deterministic, machine-independent results.
inline constexpr const char* kDefaultProcRoot = "/proc";
inline constexpr const char* kDefaultSysRoot = "/sys";

} // namespace taskmanager::datasource
