#include "PerformanceView.h"

#include <algorithm>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "../SystemMonitor.h"
#include "../util/Formatting.h"
#include "LineGraphWidget.h"

namespace taskmanager {

namespace {

constexpr int kFirstGpuIndex = 0;
// Matches Task Manager's per-metric accent colors instead of using one
// blue for everything: CPU/Memory blue, Disk green, Network red, GPU teal.
const QColor kAccentBlue(QStringLiteral("#0078d4"));
const QColor kSecondaryPurple(QStringLiteral("#8764b8"));
const QColor kAccentGreen(QStringLiteral("#107c10"));
const QColor kAccentRed(QStringLiteral("#e74856"));
const QColor kAccentTeal(QStringLiteral("#038387"));

QWidget* buildNavItem(const QString& title, QLabel** valueLabelOut, LineGraphWidget** thumbOut) {
    auto* container = new QWidget();
    auto* outer = new QHBoxLayout(container);
    outer->setContentsMargins(12, 8, 10, 8);
    outer->setSpacing(10);

    auto* thumb = new LineGraphWidget(container, /*compact=*/true);
    thumb->setFixedSize(50, 38);
    outer->addWidget(thumb);

    auto* textCol = new QVBoxLayout();
    textCol->setSpacing(2);
    auto* titleLabel = new QLabel(title, container);
    titleLabel->setStyleSheet(QStringLiteral("font-weight: 600; color: #1a1a1a; background: transparent;"));
    auto* valueLabel = new QLabel(QStringLiteral("—"), container);
    valueLabel->setStyleSheet(QStringLiteral("color: #5f5f5f; font-size: 11px; background: transparent;"));
    textCol->addWidget(titleLabel);
    textCol->addWidget(valueLabel);
    outer->addLayout(textCol, 1);

    *valueLabelOut = valueLabel;
    *thumbOut = thumb;
    return container;
}

QLabel* buildHeader(QVBoxLayout* pageLayout, const QString& title, bool withBigValue) {
    auto* header = new QHBoxLayout();
    auto* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(QStringLiteral("font-size: 20px; font-weight: 600; color: #1a1a1a;"));
    header->addWidget(titleLabel);
    header->addStretch(1);

    QLabel* bigValue = nullptr;
    if (withBigValue) {
        bigValue = new QLabel(QStringLiteral("—"));
        bigValue->setStyleSheet(QStringLiteral("font-size: 24px; font-weight: 600; color: #1a1a1a;"));
        header->addWidget(bigValue);
    }

    pageLayout->addLayout(header);
    return bigValue;
}

QGridLayout* buildStatsGrid(QVBoxLayout* pageLayout) {
    auto* grid = new QGridLayout();
    grid->setHorizontalSpacing(32);
    grid->setVerticalSpacing(8);
    grid->setContentsMargins(2, 12, 2, 0);
    pageLayout->addLayout(grid);
    pageLayout->addStretch(0);
    return grid;
}

QLabel* addGridStat(QGridLayout* grid, int row, int col, const QString& key, const QColor& dotColor = QColor()) {
    auto* keyLabel = new QLabel(key);
    if (dotColor.isValid()) {
        keyLabel->setStyleSheet(QStringLiteral("color: #5f5f5f; border-left: 3px solid %1; padding-left: 6px;")
                                     .arg(dotColor.name()));
    } else {
        keyLabel->setStyleSheet(QStringLiteral("color: #5f5f5f;"));
    }

    auto* valueLabel = new QLabel(QStringLiteral("—"));
    valueLabel->setStyleSheet(QStringLiteral("font-weight: 600;"));

    auto* pairLayout = new QVBoxLayout();
    pairLayout->setSpacing(0);
    pairLayout->addWidget(keyLabel);
    pairLayout->addWidget(valueLabel);

    grid->addLayout(pairLayout, row, col);
    return valueLabel;
}

QWidget* buildCompositionSegment(QHBoxLayout* barLayout, const QColor& color, int initialStretch) {
    auto* segment = new QWidget();
    segment->setFixedHeight(14);
    segment->setStyleSheet(QStringLiteral("background: %1;").arg(color.name()));
    barLayout->addWidget(segment, initialStretch);
    return segment;
}

} // namespace

PerformanceView::PerformanceView(SystemMonitor* monitor, QWidget* parent) : QWidget(parent) {
    auto* nav = new QListWidget(this);
    nav->setObjectName(QStringLiteral("performanceNav"));
    nav->setFixedWidth(210);
    nav->setFrameShape(QFrame::NoFrame);
    nav->setFocusPolicy(Qt::NoFocus);
    nav->setStyleSheet(QStringLiteral(
        "QListWidget { background: #ffffff; border: none; border-right: 1px solid #e5e5e5; }"
        "QListWidget::item { border-left: 3px solid transparent; }"
        "QListWidget::item:selected { background: #f0f0f0; border-left: 3px solid #0078d4; }"
        "QListWidget::item:hover:!selected { background: #f7f7f7; }"));

    auto* pages = new QStackedWidget(this);

    // --- CPU ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        cpuHeaderValue_ = buildHeader(layout, tr("CPU"), true);

        cpuGraph_ = new LineGraphWidget(page);
        cpuGraph_->setTimeAxisCaption(tr("60 segundos"), tr("0"));
        cpuSeries_ = cpuGraph_->addSeries(tr("Uso total"), kAccentBlue, 100.0, /*areaFill=*/true);
        layout->addWidget(cpuGraph_, 1);

        auto* grid = buildStatsGrid(layout);
        cpuCoresStat_ = addGridStat(grid, 0, 0, tr("Núcleos lógicos"));
        cpuProcessesStat_ = addGridStat(grid, 0, 1, tr("Processos"));

        pages->addWidget(page);
    }

    // --- Memory ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        memoryHeaderValue_ = buildHeader(layout, tr("Memória"), true);

        memoryGraph_ = new LineGraphWidget(page);
        memoryGraph_->setTimeAxisCaption(tr("60 segundos"), tr("0"));
        memorySeries_ = memoryGraph_->addSeries(tr("Em uso"), kAccentBlue, 100.0, /*areaFill=*/true);
        layout->addWidget(memoryGraph_, 1);

        auto* compositionBar = new QWidget(page);
        memoryCompositionLayout_ = new QHBoxLayout(compositionBar);
        memoryCompositionLayout_->setContentsMargins(0, 10, 0, 0);
        memoryCompositionLayout_->setSpacing(2);
        memoryUsedSegment_ = buildCompositionSegment(memoryCompositionLayout_, kAccentBlue, 1);
        memoryCachedSegment_ = buildCompositionSegment(memoryCompositionLayout_, kSecondaryPurple, 1);
        memoryFreeSegment_ = buildCompositionSegment(memoryCompositionLayout_, QColor(QStringLiteral("#e5e5e5")), 1);
        layout->addWidget(compositionBar);

        auto* grid = buildStatsGrid(layout);
        memoryUsedStat_ = addGridStat(grid, 0, 0, tr("Em uso"), kAccentBlue);
        memoryAvailableStat_ = addGridStat(grid, 0, 1, tr("Disponível"));
        memoryCachedStat_ = addGridStat(grid, 1, 0, tr("Em cache"), kSecondaryPurple);
        memoryTotalStat_ = addGridStat(grid, 1, 1, tr("Total"));

        pages->addWidget(page);
    }

    // --- Disk --- (two stacked graphs, like Task Manager's disk page)
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        diskHeaderValue_ = buildHeader(layout, tr("Disco"), true);

        auto* activeCaption = new QLabel(tr("Tempo ativo"), page);
        activeCaption->setStyleSheet(QStringLiteral("color: #5f5f5f; font-size: 11px;"));
        layout->addWidget(activeCaption);
        diskActiveGraph_ = new LineGraphWidget(page);
        diskActiveGraph_->setTimeAxisCaption(tr("60 segundos"), tr("0"));
        diskActiveSeries_ = diskActiveGraph_->addSeries(tr("Tempo ativo"), kAccentGreen, 100.0, /*areaFill=*/true);
        layout->addWidget(diskActiveGraph_, 1);

        auto* rateCaption = new QLabel(tr("Taxa de transferência"), page);
        rateCaption->setStyleSheet(QStringLiteral("color: #5f5f5f; font-size: 11px; margin-top: 8px;"));
        layout->addWidget(rateCaption);
        diskRateGraph_ = new LineGraphWidget(page);
        diskRateGraph_->setTimeAxisCaption(tr("60 segundos"), tr("0"));
        diskRateSeries_ = diskRateGraph_->addSeries(tr("Total"), kAccentGreen);
        layout->addWidget(diskRateGraph_, 1);

        auto* grid = buildStatsGrid(layout);
        diskReadStat_ = addGridStat(grid, 0, 0, tr("Leitura"));
        diskWriteStat_ = addGridStat(grid, 0, 1, tr("Escrita"));

        pages->addWidget(page);
    }

    // --- Network ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        buildHeader(layout, tr("Rede"), false);

        networkGraph_ = new LineGraphWidget(page);
        networkGraph_->setTimeAxisCaption(tr("60 segundos"), tr("0"));
        networkRxSeries_ = networkGraph_->addSeries(tr("Recebido"), kAccentRed);
        networkTxSeries_ =
            networkGraph_->addSeries(tr("Enviado"), kAccentRed, -1.0, /*areaFill=*/false, Qt::DashLine);
        layout->addWidget(networkGraph_, 1);

        auto* grid = buildStatsGrid(layout);
        networkRxStat_ = addGridStat(grid, 0, 0, tr("Recebido"), kAccentRed);
        networkTxStat_ = addGridStat(grid, 0, 1, tr("Enviado"), kAccentRed.darker(130));

        pages->addWidget(page);
    }

    // --- GPU ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        gpuHeaderValue_ = buildHeader(layout, tr("GPU"), true);

        gpuGraph_ = new LineGraphWidget(page);
        gpuGraph_->setTimeAxisCaption(tr("60 segundos"), tr("0"));
        gpuSeries_ = gpuGraph_->addSeries(tr("Uso"), kAccentTeal, 100.0, /*areaFill=*/true);
        layout->addWidget(gpuGraph_, 1);

        auto* grid = buildStatsGrid(layout);
        gpuVramStat_ = addGridStat(grid, 0, 0, tr("VRAM"));

        pages->addWidget(page);
    }

    const QStringList navTitles = {tr("CPU"), tr("Memória"), tr("Disco"), tr("Rede"), tr("GPU")};
    QLabel** navValueSlots[] = {&cpuNavValue_, &memoryNavValue_, &diskNavValue_, &networkNavValue_, &gpuNavValue_};
    LineGraphWidget** navThumbSlots[] = {&cpuThumb_, &memoryThumb_, &diskThumb_, &networkThumb_, &gpuThumb_};
    for (int i = 0; i < navTitles.size(); ++i) {
        auto* item = new QListWidgetItem();
        nav->addItem(item);
        QWidget* itemWidget = buildNavItem(navTitles[i], navValueSlots[i], navThumbSlots[i]);
        item->setSizeHint(QSize(nav->width(), 60));
        nav->setItemWidget(item, itemWidget);
    }

    cpuThumbSeries_ = cpuThumb_->addSeries(tr("Uso total"), kAccentBlue, 100.0, /*areaFill=*/true);
    memoryThumbSeries_ = memoryThumb_->addSeries(tr("Em uso"), kAccentBlue, 100.0, /*areaFill=*/true);
    diskThumbSeries_ = diskThumb_->addSeries(tr("Tempo ativo"), kAccentGreen, 100.0, /*areaFill=*/true);
    networkThumbSeries_ = networkThumb_->addSeries(tr("Recebido"), kAccentRed, -1.0, /*areaFill=*/true);
    gpuThumbSeries_ = gpuThumb_->addSeries(tr("Uso"), kAccentTeal, 100.0, /*areaFill=*/true);

    connect(nav, &QListWidget::currentRowChanged, pages, &QStackedWidget::setCurrentIndex);
    nav->setCurrentRow(0);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(nav);
    layout->addWidget(pages, 1);

    connect(monitor, &SystemMonitor::cpuSampleReady, this,
            [this](double totalPercent, const std::vector<double>& perCorePercent) {
                cpuGraph_->addSample(cpuSeries_, totalPercent);
                cpuThumb_->addSample(cpuThumbSeries_, totalPercent);
                const QString text = QString::fromStdString(util::formatPercent(totalPercent));
                cpuNavValue_->setText(text);
                cpuHeaderValue_->setText(text);
                cpuCoresStat_->setText(QString::number(perCorePercent.size()));
            });

    connect(monitor, &SystemMonitor::processesUpdated, this, [this](const std::vector<ProcessSample>& processes) {
        cpuProcessesStat_->setText(QString::number(processes.size()));
    });

    connect(monitor, &SystemMonitor::memorySampleReady, this, [this](const datasource::MemorySample& sample) {
        const double percent = sample.totalKb > 0
                                    ? (static_cast<double>(sample.usedKb()) / static_cast<double>(sample.totalKb)) * 100.0
                                    : 0.0;
        memoryGraph_->addSample(memorySeries_, percent);
        memoryThumb_->addSample(memoryThumbSeries_, percent);
        const QString text = QString::fromStdString(util::formatPercent(percent));
        memoryNavValue_->setText(text);
        memoryHeaderValue_->setText(text);

        memoryUsedStat_->setText(QString::fromStdString(util::formatBytes(sample.usedKb() * 1024)));
        memoryAvailableStat_->setText(QString::fromStdString(util::formatBytes(sample.availableKb * 1024)));
        memoryCachedStat_->setText(QString::fromStdString(util::formatBytes(sample.cachedKb * 1024)));
        memoryTotalStat_->setText(QString::fromStdString(util::formatBytes(sample.totalKb * 1024)));

        if (sample.totalKb > 0) {
            const std::uint64_t freeKb =
                sample.totalKb > (sample.usedKb() + sample.cachedKb) ? sample.totalKb - sample.usedKb() - sample.cachedKb : 0;
            memoryCompositionLayout_->setStretchFactor(memoryUsedSegment_, static_cast<int>(sample.usedKb()) + 1);
            memoryCompositionLayout_->setStretchFactor(memoryCachedSegment_, static_cast<int>(sample.cachedKb) + 1);
            memoryCompositionLayout_->setStretchFactor(memoryFreeSegment_, static_cast<int>(freeKb) + 1);
        }
    });

    connect(monitor, &SystemMonitor::diskSampleReady, this, [this](const std::vector<DiskRate>& rates) {
        double totalRead = 0.0;
        double totalWrite = 0.0;
        double maxActivePercent = 0.0;
        for (const DiskRate& rate : rates) {
            totalRead += rate.readBytesPerSec;
            totalWrite += rate.writeBytesPerSec;
            maxActivePercent = std::max(maxActivePercent, rate.activePercent);
        }
        diskActiveGraph_->addSample(diskActiveSeries_, maxActivePercent);
        diskRateGraph_->addSample(diskRateSeries_, totalRead + totalWrite);
        diskThumb_->addSample(diskThumbSeries_, maxActivePercent);

        const QString activeText = QString::fromStdString(util::formatPercent(maxActivePercent));
        const QString readText = QString::fromStdString(util::formatBytes(static_cast<std::uint64_t>(totalRead))) + "/s";
        const QString writeText =
            QString::fromStdString(util::formatBytes(static_cast<std::uint64_t>(totalWrite))) + "/s";
        diskNavValue_->setText(activeText);
        diskHeaderValue_->setText(activeText);
        diskReadStat_->setText(readText);
        diskWriteStat_->setText(writeText);
    });

    connect(monitor, &SystemMonitor::networkSampleReady, this, [this](const std::vector<NetworkRate>& rates) {
        double totalRx = 0.0;
        double totalTx = 0.0;
        for (const NetworkRate& rate : rates) {
            if (rate.name == "lo") continue; // loopback isn't meaningful for the graph
            totalRx += rate.rxBytesPerSec;
            totalTx += rate.txBytesPerSec;
        }
        networkGraph_->addSample(networkRxSeries_, totalRx);
        networkGraph_->addSample(networkTxSeries_, totalTx);
        networkThumb_->addSample(networkThumbSeries_, totalRx);

        const QString rxText = QString::fromStdString(util::formatBytes(static_cast<std::uint64_t>(totalRx))) + "/s";
        const QString txText = QString::fromStdString(util::formatBytes(static_cast<std::uint64_t>(totalTx))) + "/s";
        networkNavValue_->setText(rxText);
        networkRxStat_->setText(rxText);
        networkTxStat_->setText(txText);
    });

    connect(monitor, &SystemMonitor::gpuSampleReady, this, [this](const std::vector<datasource::GpuSample>& samples) {
        const bool hasData = !samples.empty() && samples[kFirstGpuIndex].usagePercent >= 0;
        const double usage = hasData ? static_cast<double>(samples[kFirstGpuIndex].usagePercent) : 0.0;
        gpuGraph_->addSample(gpuSeries_, usage);
        gpuThumb_->addSample(gpuThumbSeries_, usage);

        const QString text = hasData ? QString::fromStdString(util::formatPercent(usage)) : tr("N/D");
        gpuNavValue_->setText(text);
        gpuHeaderValue_->setText(text);

        if (hasData && samples[kFirstGpuIndex].vramTotalBytes > 0) {
            gpuVramStat_->setText(QStringLiteral("%1 / %2")
                                       .arg(QString::fromStdString(util::formatBytes(samples[kFirstGpuIndex].vramUsedBytes)))
                                       .arg(QString::fromStdString(util::formatBytes(samples[kFirstGpuIndex].vramTotalBytes))));
        } else {
            gpuVramStat_->setText(tr("Sem dados"));
        }
    });
}

} // namespace taskmanager
