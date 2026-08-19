#include "GpuStats.h"

#include <charconv>

#include "../util/ProcFileUtils.h"

namespace taskmanager::datasource {

using taskmanager::util::readFile;

namespace {
std::optional<std::uint64_t> parseU64(const std::string& content) {
    std::string trimmed = content;
    while (!trimmed.empty() && (trimmed.back() == '\n' || trimmed.back() == ' ')) {
        trimmed.pop_back();
    }
    std::uint64_t value = 0;
    auto [ptr, ec] = std::from_chars(trimmed.data(), trimmed.data() + trimmed.size(), value);
    if (ec != std::errc()) return std::nullopt;
    return value;
}
} // namespace

GpuStats::GpuStats(std::filesystem::path sysRoot) : sysRoot_(std::move(sysRoot)) {}

std::vector<GpuSample> GpuStats::sample() const {
    std::vector<GpuSample> result;

    const std::filesystem::path drmRoot = sysRoot_ / "class" / "drm";
    std::error_code ec;
    if (!std::filesystem::exists(drmRoot, ec)) return result;

    for (const auto& entry : std::filesystem::directory_iterator(drmRoot, ec)) {
        const std::string cardName = entry.path().filename().string();
        if (cardName.rfind("card", 0) != 0 || cardName.find('-') != std::string::npos) {
            continue; // skip connector entries like "card0-DP-1"
        }

        const std::filesystem::path deviceDir = entry.path() / "device";
        const auto busy = readFile(deviceDir / "gpu_busy_percent");
        const auto vramUsed = readFile(deviceDir / "mem_info_vram_used");
        const auto vramTotal = readFile(deviceDir / "mem_info_vram_total");

        if (!busy && !vramUsed && !vramTotal) {
            continue; // this driver doesn't expose any counter we understand
        }

        GpuSample sample;
        sample.cardName = cardName;
        if (busy) {
            if (auto v = parseU64(*busy)) sample.usagePercent = static_cast<int>(*v);
        }
        if (vramUsed) {
            if (auto v = parseU64(*vramUsed)) sample.vramUsedBytes = *v;
        }
        if (vramTotal) {
            if (auto v = parseU64(*vramTotal)) sample.vramTotalBytes = *v;
        }
        result.push_back(std::move(sample));
    }

    return result;
}

} // namespace taskmanager::datasource
