#pragma once

#include <QWidget>
#include <vector>

QT_BEGIN_NAMESPACE
class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;
QT_END_NAMESPACE

namespace taskmanager {

// Generic rolling real-time line chart, shared by every PerformanceView
// page (CPU, Memory, Disk, Network, GPU) instead of one bespoke widget
// class per metric — they only differ in title, series names, and whether
// the Y axis is a fixed 0-100% or auto-scaled to the observed max (e.g.
// bytes/sec).
class LineGraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit LineGraphWidget(const QString& title, QWidget* parent = nullptr);

    // yAxisMax >= 0 fixes the Y axis (e.g. 100 for a percentage); a
    // negative value auto-scales the shared Y axis to the largest sample
    // observed so far across all series on this graph.
    int addSeries(const QString& name, double yAxisMax = -1.0);

    void addSample(int seriesIndex, double value);

private:
    static constexpr int kWindowSize = 60;

    QChart* chart_ = nullptr;
    QChartView* chartView_ = nullptr;
    QValueAxis* axisX_ = nullptr;
    QValueAxis* axisY_ = nullptr;
    std::vector<QLineSeries*> series_;
    double fixedYMax_ = -1.0;
    double observedYMax_ = 1.0;
    int sampleCount_ = 0;
};

} // namespace taskmanager
