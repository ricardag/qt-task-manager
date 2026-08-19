#pragma once

#include <QColor>
#include <QWidget>
#include <vector>

QT_BEGIN_NAMESPACE
class QChart;
class QChartView;
class QLineSeries;
class QAreaSeries;
class QValueAxis;
QT_END_NAMESPACE

namespace taskmanager {

// Generic rolling real-time line chart, shared by every PerformanceView
// page (CPU, Memory, Disk, Network, GPU) instead of one bespoke widget
// class per metric. Styled to match Task Manager's minimal look: no
// title/legend/border on the chart itself (PerformanceView renders those
// as plain widgets around it), Y axis on the right, gridlines only.
class LineGraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit LineGraphWidget(QWidget* parent = nullptr);

    // yAxisMax >= 0 fixes the Y axis (e.g. 100 for a percentage); a
    // negative value auto-scales the shared Y axis to the largest sample
    // observed so far across all series on this graph. areaFill draws a
    // gradient fill under the line down to zero, matching Task Manager's
    // single-metric graphs (CPU, Memory, GPU); leave it off for graphs
    // with two comparable series (Disk read/write, Network rx/tx).
    int addSeries(const QString& name, const QColor& color, double yAxisMax = -1.0, bool areaFill = false);

    void addSample(int seriesIndex, double value);

private:
    static constexpr int kWindowSize = 60;

    QChart* chart_ = nullptr;
    QChartView* chartView_ = nullptr;
    QValueAxis* axisX_ = nullptr;
    QValueAxis* axisY_ = nullptr;
    std::vector<QLineSeries*> dataSeries_; // holds the actual point data for every series
    double fixedYMax_ = -1.0;
    double observedYMax_ = 1.0;
    int sampleCount_ = 0;
};

} // namespace taskmanager
