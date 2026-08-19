#pragma once

#include <QWidget>

namespace taskmanager {

class SystemMonitor;
class LineGraphWidget;

// Win11-style master/detail layout: a list of metrics on the left (CPU,
// Memória, Disco, Rede, GPU), a rolling line chart for the selected one on
// the right. Each chart is a LineGraphWidget fed by SystemMonitor's
// signals — this view never touches datasource/ directly.
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
};

} // namespace taskmanager
