#include "SystemMonitor.h"

#include <QTimer>

namespace taskmanager {

using datasource::CpuCoreTicks;
using datasource::DiskDeviceSample;
using datasource::NetworkInterfaceSample;
using datasource::ProcessInfo;

SystemMonitor::SystemMonitor(QObject* parent) : QObject(parent) {
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &SystemMonitor::tick);
}

void SystemMonitor::start(int intervalMs) {
    intervalSeconds_ = intervalMs / 1000.0;
    timer_->start(intervalMs);
    tick(); // populate views immediately instead of waiting for the first interval
}

void SystemMonitor::stop() {
    timer_->stop();
}

void SystemMonitor::tick() {
    // --- CPU ---
    const std::vector<CpuCoreTicks> currentCpuTicks = cpuStats_.sampleTicks();
    if (!previousCpuTicks_.empty() && previousCpuTicks_.size() == currentCpuTicks.size()) {
        const double totalPercent = datasource::CpuStats::usagePercent(previousCpuTicks_[0], currentCpuTicks[0]);
        std::vector<double> perCorePercent;
        perCorePercent.reserve(currentCpuTicks.size() - 1);
        for (std::size_t i = 1; i < currentCpuTicks.size(); ++i) {
            perCorePercent.push_back(datasource::CpuStats::usagePercent(previousCpuTicks_[i], currentCpuTicks[i]));
        }
        emit cpuSampleReady(totalPercent, perCorePercent);
    }

    // --- Memory ---
    emit memorySampleReady(memoryStats_.sample());

    // --- Network ---
    const std::vector<NetworkInterfaceSample> currentNetSamples = networkStats_.sample();
    std::vector<NetworkRate> netRates;
    netRates.reserve(currentNetSamples.size());
    for (const NetworkInterfaceSample& current : currentNetSamples) {
        NetworkRate rate;
        rate.name = current.name;
        const auto it = previousNetworkSamples_.find(current.name);
        if (it != previousNetworkSamples_.end() && intervalSeconds_ > 0) {
            const auto& prev = it->second;
            rate.rxBytesPerSec = static_cast<double>(current.rxBytes - prev.rxBytes) / intervalSeconds_;
            rate.txBytesPerSec = static_cast<double>(current.txBytes - prev.txBytes) / intervalSeconds_;
        }
        netRates.push_back(rate);
    }
    emit networkSampleReady(netRates);
    previousNetworkSamples_.clear();
    for (const auto& sample : currentNetSamples) {
        previousNetworkSamples_.emplace(sample.name, sample);
    }

    // --- Disk ---
    const std::vector<DiskDeviceSample> currentDiskSamples = diskStats_.sampleDevices();
    std::vector<DiskRate> diskRates;
    diskRates.reserve(currentDiskSamples.size());
    static constexpr double kSectorBytes = 512.0;
    for (const DiskDeviceSample& current : currentDiskSamples) {
        DiskRate rate;
        rate.name = current.name;
        const auto it = previousDiskSamples_.find(current.name);
        if (it != previousDiskSamples_.end() && intervalSeconds_ > 0) {
            const auto& prev = it->second;
            rate.readBytesPerSec =
                static_cast<double>(current.sectorsRead - prev.sectorsRead) * kSectorBytes / intervalSeconds_;
            rate.writeBytesPerSec =
                static_cast<double>(current.sectorsWritten - prev.sectorsWritten) * kSectorBytes / intervalSeconds_;
        }
        diskRates.push_back(rate);
    }
    emit diskSampleReady(diskRates);
    previousDiskSamples_.clear();
    for (const auto& sample : currentDiskSamples) {
        previousDiskSamples_.emplace(sample.name, sample);
    }

    // --- GPU (best-effort, may be empty) ---
    emit gpuSampleReady(gpuStats_.sample());

    // --- Processes ---
    const std::vector<ProcessInfo> currentProcesses = processReader_.readAll();
    std::vector<ProcessSample> processSamples;
    processSamples.reserve(currentProcesses.size());
    std::unordered_map<int, std::uint64_t> currentProcessTicks;
    currentProcessTicks.reserve(currentProcesses.size());

    const double systemTotalDelta = (!previousCpuTicks_.empty() && !currentCpuTicks.empty())
                                         ? static_cast<double>(currentCpuTicks[0].total() - previousCpuTicks_[0].total())
                                         : 0.0;

    for (const ProcessInfo& info : currentProcesses) {
        const std::uint64_t ticks = info.utimeTicks + info.stimeTicks;
        currentProcessTicks[info.pid] = ticks;

        ProcessSample sample;
        sample.info = info;
        const auto prevIt = previousProcessTicks_.find(info.pid);
        if (prevIt != previousProcessTicks_.end() && systemTotalDelta > 0.0) {
            const double delta = static_cast<double>(ticks - prevIt->second);
            sample.cpuPercent = (delta / systemTotalDelta) * 100.0;
        }
        processSamples.push_back(std::move(sample));
    }
    emit processesUpdated(processSamples);
    previousProcessTicks_ = std::move(currentProcessTicks);

    previousCpuTicks_ = currentCpuTicks;
}

} // namespace taskmanager
