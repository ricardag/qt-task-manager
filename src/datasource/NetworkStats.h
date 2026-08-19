#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "ProcFsPaths.h"

namespace taskmanager::datasource {

struct NetworkInterfaceSample {
    std::string name;
    std::uint64_t rxBytes = 0;
    std::uint64_t rxPackets = 0;
    std::uint64_t txBytes = 0;
    std::uint64_t txPackets = 0;
};

class NetworkStats {
public:
    explicit NetworkStats(std::filesystem::path procRoot = std::filesystem::path(kDefaultProcRoot));

    std::vector<NetworkInterfaceSample> sample() const;

private:
    std::filesystem::path procRoot_;
};

} // namespace taskmanager::datasource
