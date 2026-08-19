#include "LineGraphWidget.h"

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QVBoxLayout>
#include <algorithm>

QT_USE_NAMESPACE

namespace taskmanager {

LineGraphWidget::LineGraphWidget(const QString& title, QWidget* parent) : QWidget(parent) {
    chart_ = new QChart();
    chart_->setTitle(title);
    chart_->legend()->setVisible(true);
    chart_->legend()->setAlignment(Qt::AlignBottom);

    axisX_ = new QValueAxis(chart_);
    axisX_->setLabelsVisible(false);
    axisX_->setRange(0, kWindowSize);
    chart_->addAxis(axisX_, Qt::AlignBottom);

    axisY_ = new QValueAxis(chart_);
    axisY_->setRange(0, 1);
    chart_->addAxis(axisY_, Qt::AlignLeft);

    chartView_ = new QChartView(chart_, this);
    chartView_->setRenderHint(QPainter::Antialiasing);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView_);
}

int LineGraphWidget::addSeries(const QString& name, double yAxisMax) {
    auto* series = new QLineSeries(chart_);
    series->setName(name);
    chart_->addSeries(series);
    series->attachAxis(axisX_);
    series->attachAxis(axisY_);

    if (yAxisMax >= 0.0) {
        fixedYMax_ = yAxisMax;
        axisY_->setRange(0, fixedYMax_);
    }

    series_.push_back(series);
    return static_cast<int>(series_.size()) - 1;
}

void LineGraphWidget::addSample(int seriesIndex, double value) {
    if (seriesIndex < 0 || seriesIndex >= static_cast<int>(series_.size())) {
        return;
    }

    QLineSeries* series = series_[seriesIndex];
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
    if (seriesIndex == static_cast<int>(series_.size()) - 1) {
        ++sampleCount_;
        if (sampleCount_ > kWindowSize) {
            axisX_->setRange(sampleCount_ - kWindowSize, sampleCount_);
        }
    }
}

} // namespace taskmanager
