#pragma once

#include <QObject>
#include <unordered_map>
#include <vector>

#include "datasource/CpuStats.h"
#include "datasource/DiskStats.h"
#include "datasource/GpuStats.h"
#include "datasource/MemoryStats.h"
#include "datasource/NetworkStats.h"
#include "datasource/ProcessInfo.h"
#include "datasource/ProcessReader.h"

namespace taskmanager {

// A process snapshot with CPU usage already resolved to a percentage of
// total system capacity (matching Task Manager's normalization: one fully
// busy core on a 4-core machine reads as 25%, not 100%).
struct ProcessSample {
    datasource::ProcessInfo info;
    double cpuPercent = 0.0;
};

struct NetworkRate {
    std::string name;
    double rxBytesPerSec = 0.0;
    double txBytesPerSec = 0.0;
};

struct DiskRate {
    std::string name;
    double readBytesPerSec = 0.0;
    double writeBytesPerSec = 0.0;
};

// Central facade that owns the single polling QTimer and delegates each
// tick to the datasource/ readers, turning their raw/cumulative counters
// into the per-interval percentages and rates the views actually display.
// Views never read /proc directly; they only connect to these signals.
class SystemMonitor : public QObject {
    Q_OBJECT

public:
    explicit SystemMonitor(QObject* parent = nullptr);

    void start(int intervalMs = 1000);
    void stop();

signals:
    void processesUpdated(const std::vector<ProcessSample>& processes);
    void cpuSampleReady(double totalPercent, const std::vector<double>& perCorePercent);
    void memorySampleReady(const datasource::MemorySample& sample);
    void networkSampleReady(const std::vector<NetworkRate>& rates);
    void diskSampleReady(const std::vector<DiskRate>& rates);
    void gpuSampleReady(const std::vector<datasource::GpuSample>& samples);

private slots:
    void tick();

private:
    datasource::CpuStats cpuStats_;
    datasource::MemoryStats memoryStats_;
    datasource::DiskStats diskStats_;
    datasource::NetworkStats networkStats_;
    datasource::GpuStats gpuStats_;
    datasource::ProcessReader processReader_;

    class QTimer* timer_ = nullptr;
    double intervalSeconds_ = 1.0;

    std::vector<datasource::CpuCoreTicks> previousCpuTicks_;
    std::unordered_map<int, std::uint64_t> previousProcessTicks_; // pid -> utime+stime
    std::unordered_map<std::string, datasource::NetworkInterfaceSample> previousNetworkSamples_;
    std::unordered_map<std::string, datasource::DiskDeviceSample> previousDiskSamples_;
};

} // namespace taskmanager
