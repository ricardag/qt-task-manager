#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

namespace taskmanager {

class SystemMonitor;
class LineGraphWidget;

// Win11-style master/detail layout: a compact list of metrics on the left
// (CPU, Memória, Disco, Rede, GPU), each row showing a live thumbnail
// sparkline plus its current reading, and a rolling line chart for the
// selected one on the right with a header (title + big current value), a
// grid of stats underneath, and (Memory only) a composition bar. Every
// number comes from SystemMonitor's signals — this view never touches
// datasource/ directly.
class PerformanceView : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceView(SystemMonitor* monitor, QWidget* parent = nullptr);

private:
    LineGraphWidget* cpuGraph_ = nullptr;
    LineGraphWidget* memoryGraph_ = nullptr;
    LineGraphWidget* diskActiveGraph_ = nullptr; // "Tempo ativo" %, like Task Manager's disk page
    LineGraphWidget* diskRateGraph_ = nullptr;   // combined read+write transfer rate below it
    LineGraphWidget* networkGraph_ = nullptr;
    LineGraphWidget* gpuGraph_ = nullptr;

    int cpuSeries_ = -1;
    int memorySeries_ = -1;
    int diskActiveSeries_ = -1;
    int diskRateSeries_ = -1;
    int networkRxSeries_ = -1;
    int networkTxSeries_ = -1;
    int gpuSeries_ = -1;

    // Live thumbnail sparkline shown next to each item in the metric list.
    LineGraphWidget* cpuThumb_ = nullptr;
    LineGraphWidget* memoryThumb_ = nullptr;
    LineGraphWidget* diskThumb_ = nullptr;
    LineGraphWidget* networkThumb_ = nullptr;
    LineGraphWidget* gpuThumb_ = nullptr;
    int cpuThumbSeries_ = -1;
    int memoryThumbSeries_ = -1;
    int diskThumbSeries_ = -1;
    int networkThumbSeries_ = -1;
    int gpuThumbSeries_ = -1;

    // Small live value shown next to each item in the left-hand metric list.
    QLabel* cpuNavValue_ = nullptr;
    QLabel* memoryNavValue_ = nullptr;
    QLabel* diskNavValue_ = nullptr;
    QLabel* networkNavValue_ = nullptr;
    QLabel* gpuNavValue_ = nullptr;

    // Big current-value readout in each page's header.
    QLabel* cpuHeaderValue_ = nullptr;
    QLabel* memoryHeaderValue_ = nullptr;
    QLabel* diskHeaderValue_ = nullptr;
    QLabel* gpuHeaderValue_ = nullptr;

    // Memory composition bar segments (Em uso / Cache / Livre).
    QHBoxLayout* memoryCompositionLayout_ = nullptr;
    QWidget* memoryUsedSegment_ = nullptr;
    QWidget* memoryCachedSegment_ = nullptr;
    QWidget* memoryFreeSegment_ = nullptr;

    // Stats grid underneath each graph.
    QLabel* cpuCoresStat_ = nullptr;
    QLabel* cpuProcessesStat_ = nullptr;
    QLabel* memoryUsedStat_ = nullptr;
    QLabel* memoryAvailableStat_ = nullptr;
    QLabel* memoryCachedStat_ = nullptr;
    QLabel* memoryTotalStat_ = nullptr;
    QLabel* diskReadStat_ = nullptr;
    QLabel* diskWriteStat_ = nullptr;
    QLabel* networkRxStat_ = nullptr;
    QLabel* networkTxStat_ = nullptr;
    QLabel* gpuVramStat_ = nullptr;
};

} // namespace taskmanager
