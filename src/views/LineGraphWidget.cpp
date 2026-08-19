#include "LineGraphWidget.h"

#include <QAreaSeries>
#include <QChart>
#include <QChartView>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineSeries>
#include <QValueAxis>
#include <QVBoxLayout>
#include <algorithm>

QT_USE_NAMESPACE

namespace taskmanager {

LineGraphWidget::LineGraphWidget(QWidget* parent, bool compact) : QWidget(parent), compact_(compact) {
    chart_ = new QChart();
    chart_->legend()->hide();
    chart_->setBackgroundVisible(false);
    chart_->setPlotAreaBackgroundVisible(false);
    chart_->setMargins(compact_ ? QMargins(0, 0, 0, 0) : QMargins(2, 8, 8, 2));

    axisX_ = new QValueAxis(chart_);
    axisX_->setLabelsVisible(false);
    axisX_->setLineVisible(false);
    axisX_->setRange(0, kWindowSize);
    if (compact_) {
        axisX_->setGridLineVisible(false);
    } else {
        // Vertical gridlines at regular intervals, matching Task
        // Manager's boxed-grid look instead of a bare line.
        axisX_->setGridLineVisible(true);
        axisX_->setGridLineColor(QColor(0, 0, 0, 20));
        axisX_->setTickCount(7);
    }
    chart_->addAxis(axisX_, Qt::AlignBottom);

    axisY_ = new QValueAxis(chart_);
    axisY_->setRange(0, 1);
    axisY_->setLineVisible(false);
    axisY_->setLabelsVisible(!compact_);
    axisY_->setGridLineVisible(!compact_);
    axisY_->setGridLineColor(QColor(0, 0, 0, 30));
    axisY_->setLabelsColor(QColor("#5f5f5f"));
    chart_->addAxis(axisY_, Qt::AlignRight);

    chartView_ = new QChartView(chart_, this);
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setFrameShape(QFrame::NoFrame);
    chartView_->setBackgroundBrush(QBrush(Qt::white));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    if (compact_) {
        layout->addWidget(chartView_);
        return;
    }

    auto* chartFrame = new QFrame(this);
    chartFrame->setObjectName(QStringLiteral("lineGraphFrame"));
    chartFrame->setStyleSheet(QStringLiteral("#lineGraphFrame { border: 1px solid #e1e1e1; background: white; }"));
    auto* frameLayout = new QVBoxLayout(chartFrame);
    frameLayout->setContentsMargins(1, 1, 1, 1);
    frameLayout->addWidget(chartView_);
    layout->addWidget(chartFrame, 1);

    auto* captionRow = new QHBoxLayout();
    captionLeft_ = new QLabel(this);
    captionRight_ = new QLabel(this);
    captionLeft_->setStyleSheet(QStringLiteral("color: #8a8886; font-size: 11px;"));
    captionRight_->setStyleSheet(QStringLiteral("color: #8a8886; font-size: 11px;"));
    captionRow->addWidget(captionLeft_);
    captionRow->addStretch(1);
    captionRow->addWidget(captionRight_);
    layout->addLayout(captionRow);
}

void LineGraphWidget::setTimeAxisCaption(const QString& leftText, const QString& rightText) {
    if (captionLeft_) captionLeft_->setText(leftText);
    if (captionRight_) captionRight_->setText(rightText);
}

int LineGraphWidget::addSeries(const QString& name, const QColor& color, double yAxisMax, bool areaFill,
                                Qt::PenStyle penStyle) {
    auto* line = new QLineSeries();
    line->setName(name);

    QPen pen(color);
    pen.setWidthF(compact_ ? 1.25 : 1.75);
    pen.setStyle(penStyle);

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
    } else if (!compact_) {
        // Auto-scaled graphs (bytes/sec) would otherwise show raw
        // unformatted numbers like "2044723.2" on the axis; the nav list,
        // header, and stats row already show the properly formatted
        // value, so just keep the gridlines as a scale reference.
        axisY_->setLabelsVisible(false);
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
        // Recompute from the points still in the visible window (rather
        // than an ever-growing max) so a one-time spike doesn't flatten
        // the rest of the graph forever once it scrolls out of view.
        double windowMax = 0.0;
        for (const QLineSeries* s : dataSeries_) {
            for (const QPointF& point : s->points()) {
                windowMax = std::max(windowMax, point.y());
            }
        }
        observedYMax_ = std::max(windowMax * 1.2, 1.0);
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
