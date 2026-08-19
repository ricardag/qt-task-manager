#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace taskmanager {

class SystemMonitor;
class LineGraphWidget;

// Win11-style master/detail layout: a compact list of metrics on the left
// (CPU, Memória, Disco, Rede, GPU) each showing its live current reading,
// a rolling line chart for the selected one on the right with a header
// (title + big current value) and a small stats row underneath. Every
// number comes from SystemMonitor's signals — this view never touches
// datasource/ directly.
class PerformanceView : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceView(SystemMonitor* monitor, QWidget* parent = nullptr);

private:
    LineGraphWidget* cpuGraph_ = nullptr;
    LineGraphWidget* memoryGraph_ = nullptr;
    LineGraphWidget* diskGraph_ = nullptr;
    LineGraphWidget* networkGraph_ = nullptr;
    LineGraphWidget* gpuGraph_ = nullptr;

    int cpuSeries_ = -1;
    int memorySeries_ = -1;
    int diskReadSeries_ = -1;
    int diskWriteSeries_ = -1;
    int networkRxSeries_ = -1;
    int networkTxSeries_ = -1;
    int gpuSeries_ = -1;

    // Small live value shown next to each item in the left-hand metric list.
    QLabel* cpuNavValue_ = nullptr;
    QLabel* memoryNavValue_ = nullptr;
    QLabel* diskNavValue_ = nullptr;
    QLabel* networkNavValue_ = nullptr;
    QLabel* gpuNavValue_ = nullptr;

    // Big current-value readout in each page's header.
    QLabel* cpuHeaderValue_ = nullptr;
    QLabel* memoryHeaderValue_ = nullptr;
    QLabel* gpuHeaderValue_ = nullptr;

    // Small stats row underneath each graph.
    QLabel* cpuCoresStat_ = nullptr;
    QLabel* cpuProcessesStat_ = nullptr;
    QLabel* memoryUsedStat_ = nullptr;
    QLabel* memoryAvailableStat_ = nullptr;
    QLabel* memoryTotalStat_ = nullptr;
    QLabel* diskReadStat_ = nullptr;
    QLabel* diskWriteStat_ = nullptr;
    QLabel* networkRxStat_ = nullptr;
    QLabel* networkTxStat_ = nullptr;
    QLabel* gpuVramStat_ = nullptr;
};

} // namespace taskmanager
