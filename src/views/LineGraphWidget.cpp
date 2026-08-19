#include "LineGraphWidget.h"

#include <QAreaSeries>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QVBoxLayout>
#include <algorithm>

QT_USE_NAMESPACE

namespace taskmanager {

LineGraphWidget::LineGraphWidget(QWidget* parent) : QWidget(parent) {
    chart_ = new QChart();
    chart_->legend()->hide();
    chart_->setBackgroundVisible(false);
    chart_->setPlotAreaBackgroundVisible(false);
    chart_->setMargins(QMargins(0, 6, 6, 0));

    axisX_ = new QValueAxis(chart_);
    axisX_->setLabelsVisible(false);
    axisX_->setGridLineVisible(false);
    axisX_->setLineVisible(false);
    axisX_->setRange(0, kWindowSize);
    chart_->addAxis(axisX_, Qt::AlignBottom);

    axisY_ = new QValueAxis(chart_);
    axisY_->setRange(0, 1);
    axisY_->setLineVisible(false);
    axisY_->setGridLineColor(QColor(0, 0, 0, 30));
    axisY_->setLabelsColor(QColor("#5f5f5f"));
    chart_->addAxis(axisY_, Qt::AlignRight);

    chartView_ = new QChartView(chart_, this);
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setFrameShape(QFrame::NoFrame);
    chartView_->setBackgroundBrush(QBrush(Qt::white));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView_);
}

int LineGraphWidget::addSeries(const QString& name, const QColor& color, double yAxisMax, bool areaFill) {
    auto* line = new QLineSeries();
    line->setName(name);

    QPen pen(color);
    pen.setWidthF(1.75);

    QAbstractSeries* rendered = line;
    if (areaFill) {
        auto* area = new QAreaSeries(line);
        area->setName(name);
        area->setPen(pen);

        QColor fillTop = color;
        fillTop.setAlpha(80);
        QColor fillBottom = color;
        fillBottom.setAlpha(0);
        QLinearGradient gradient(0, 0, 0, 1);
        gradient.setColorAt(0.0, fillTop);
        gradient.setColorAt(1.0, fillBottom);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        area->setBrush(gradient);

        rendered = area;
    } else {
        line->setPen(pen);
    }

    chart_->addSeries(rendered);
    rendered->attachAxis(axisX_);
    rendered->attachAxis(axisY_);

    if (yAxisMax >= 0.0) {
        fixedYMax_ = yAxisMax;
        axisY_->setRange(0, fixedYMax_);
    }

    dataSeries_.push_back(line);
    return static_cast<int>(dataSeries_.size()) - 1;
}

void LineGraphWidget::addSample(int seriesIndex, double value) {
    if (seriesIndex < 0 || seriesIndex >= static_cast<int>(dataSeries_.size())) {
        return;
    }

    QLineSeries* series = dataSeries_[seriesIndex];
    series->append(sampleCount_, value);

    if (series->count() > kWindowSize) {
        series->remove(0);
    }

    if (fixedYMax_ < 0.0) {
        observedYMax_ = std::max(observedYMax_, value * 1.2);
        axisY_->setRange(0, observedYMax_);
    }

    // Advance the shared X window only once all series for this tick have
    // been added; the caller drives that by calling addSample once per
    // series with the same implicit tick, then triggering a repaint.
    if (seriesIndex == static_cast<int>(dataSeries_.size()) - 1) {
        ++sampleCount_;
        if (sampleCount_ > kWindowSize) {
            axisX_->setRange(sampleCount_ - kWindowSize, sampleCount_);
        }
    }
}

} // namespace taskmanager
