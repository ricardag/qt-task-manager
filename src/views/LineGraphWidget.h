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
class QLabel;
QT_END_NAMESPACE

namespace taskmanager {

// Generic rolling real-time line chart, shared by every PerformanceView
// page (CPU, Memory, Disk, Network, GPU) instead of one bespoke widget
// class per metric, and reused a second time at a tiny fixed size as the
// live thumbnail sparkline in each metric's nav-list entry (compact=true
// in the constructor: no border/caption/axis chrome, just the line).
class LineGraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit LineGraphWidget(QWidget* parent = nullptr, bool compact = false);

    // yAxisMax >= 0 fixes the Y axis (e.g. 100 for a percentage); a
    // negative value auto-scales the shared Y axis to the largest sample
    // observed in the currently visible window. areaFill draws a gradient
    // fill under the line down to zero, matching Task Manager's
    // single-metric graphs (CPU, Memory, GPU); leave it off for graphs
    // with two comparable series (Disk read/write, Network rx/tx).
    int addSeries(const QString& name, const QColor& color, double yAxisMax = -1.0, bool areaFill = false,
                  Qt::PenStyle penStyle = Qt::SolidLine);

    void addSample(int seriesIndex, double value);

    // Sets the "60 seconds" / "0" style caption under the graph. No-op in
    // compact mode, which has no caption row.
    void setTimeAxisCaption(const QString& leftText, const QString& rightText);

private:
    static constexpr int kWindowSize = 60;

    bool compact_ = false;
    QChart* chart_ = nullptr;
    QChartView* chartView_ = nullptr;
    QValueAxis* axisX_ = nullptr;
    QValueAxis* axisY_ = nullptr;
    QLabel* captionLeft_ = nullptr;
    QLabel* captionRight_ = nullptr;
    std::vector<QLineSeries*> dataSeries_; // holds the actual point data for every series
    double fixedYMax_ = -1.0;
    double observedYMax_ = 1.0;
    int sampleCount_ = 0;
};

} // namespace taskmanager
