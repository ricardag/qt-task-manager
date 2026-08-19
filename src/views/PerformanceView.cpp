#include "PerformanceView.h"

#include <QFormLayout>
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
const QColor kAccentBlue(QStringLiteral("#0078d4"));
const QColor kSecondaryPurple(QStringLiteral("#8764b8"));

QWidget* buildNavItem(const QString& title, QLabel** valueLabelOut) {
    auto* container = new QWidget();
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(14, 6, 10, 6);
    layout->setSpacing(2);

    auto* titleLabel = new QLabel(title, container);
    titleLabel->setStyleSheet(QStringLiteral("font-weight: 600; color: #1a1a1a; background: transparent;"));

    auto* valueLabel = new QLabel(QStringLiteral("—"), container);
    valueLabel->setStyleSheet(QStringLiteral("color: #5f5f5f; font-size: 11px; background: transparent;"));

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);

    *valueLabelOut = valueLabel;
    return container;
}

QLabel* buildHeader(QVBoxLayout* pageLayout, const QString& title, bool withBigValue) {
    auto* header = new QHBoxLayout();
    auto* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: 600; color: #1a1a1a;"));
    header->addWidget(titleLabel);
    header->addStretch(1);

    QLabel* bigValue = nullptr;
    if (withBigValue) {
        bigValue = new QLabel(QStringLiteral("—"));
        bigValue->setStyleSheet(QStringLiteral("font-size: 22px; font-weight: 600; color: #0078d4;"));
        header->addWidget(bigValue);
    }

    pageLayout->addLayout(header);
    return bigValue;
}

QLabel* addStatRow(QFormLayout* form, const QString& key, const QColor& dotColor = QColor()) {
    QString keyText = key;
    auto* keyLabel = new QLabel(keyText);
    keyLabel->setStyleSheet(QStringLiteral("color: #5f5f5f;"));
    if (dotColor.isValid()) {
        keyLabel->setStyleSheet(QStringLiteral("color: #5f5f5f; border-left: 3px solid %1; padding-left: 6px;")
                                     .arg(dotColor.name()));
    }

    auto* valueLabel = new QLabel(QStringLiteral("—"));
    valueLabel->setStyleSheet(QStringLiteral("font-weight: 600;"));
    form->addRow(keyLabel, valueLabel);
    return valueLabel;
}

QFormLayout* buildStatsForm(QVBoxLayout* pageLayout) {
    auto* form = new QFormLayout();
    form->setHorizontalSpacing(28);
    form->setContentsMargins(2, 10, 2, 0);
    pageLayout->addLayout(form);
    pageLayout->addStretch(0);
    return form;
}

} // namespace

PerformanceView::PerformanceView(SystemMonitor* monitor, QWidget* parent) : QWidget(parent) {
    auto* nav = new QListWidget(this);
    nav->setObjectName(QStringLiteral("performanceNav"));
    nav->setFixedWidth(190);
    nav->setFrameShape(QFrame::NoFrame);
    nav->setFocusPolicy(Qt::NoFocus);
    nav->setStyleSheet(QStringLiteral(
        "QListWidget { background: #ffffff; border: none; border-right: 1px solid #e5e5e5; }"
        "QListWidget::item { border-left: 3px solid transparent; }"
        "QListWidget::item:selected { background: #f3f9fd; border-left: 3px solid #0078d4; }"
        "QListWidget::item:hover:!selected { background: #f5f5f5; }"));

    auto* pages = new QStackedWidget(this);

    // --- CPU ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        cpuHeaderValue_ = buildHeader(layout, tr("CPU"), true);

        cpuGraph_ = new LineGraphWidget(page);
        cpuSeries_ = cpuGraph_->addSeries(tr("Uso total"), kAccentBlue, 100.0, /*areaFill=*/true);
        layout->addWidget(cpuGraph_, 1);

        auto* form = buildStatsForm(layout);
        cpuCoresStat_ = addStatRow(form, tr("Núcleos lógicos"));
        cpuProcessesStat_ = addStatRow(form, tr("Processos"));

        pages->addWidget(page);
    }

    // --- Memory ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        memoryHeaderValue_ = buildHeader(layout, tr("Memória"), true);

        memoryGraph_ = new LineGraphWidget(page);
        memorySeries_ = memoryGraph_->addSeries(tr("Em uso"), kAccentBlue, 100.0, /*areaFill=*/true);
        layout->addWidget(memoryGraph_, 1);

        auto* form = buildStatsForm(layout);
        memoryUsedStat_ = addStatRow(form, tr("Em uso"));
        memoryAvailableStat_ = addStatRow(form, tr("Disponível"));
        memoryTotalStat_ = addStatRow(form, tr("Total"));

        pages->addWidget(page);
    }

    // --- Disk ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        buildHeader(layout, tr("Disco"), false);

        diskGraph_ = new LineGraphWidget(page);
        diskReadSeries_ = diskGraph_->addSeries(tr("Leitura"), kAccentBlue);
        diskWriteSeries_ = diskGraph_->addSeries(tr("Escrita"), kSecondaryPurple);
        layout->addWidget(diskGraph_, 1);

        auto* form = buildStatsForm(layout);
        diskReadStat_ = addStatRow(form, tr("Leitura"), kAccentBlue);
        diskWriteStat_ = addStatRow(form, tr("Escrita"), kSecondaryPurple);

        pages->addWidget(page);
    }

    // --- Network ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        buildHeader(layout, tr("Rede"), false);

        networkGraph_ = new LineGraphWidget(page);
        networkRxSeries_ = networkGraph_->addSeries(tr("Recebido"), kAccentBlue);
        networkTxSeries_ = networkGraph_->addSeries(tr("Enviado"), kSecondaryPurple);
        layout->addWidget(networkGraph_, 1);

        auto* form = buildStatsForm(layout);
        networkRxStat_ = addStatRow(form, tr("Recebido"), kAccentBlue);
        networkTxStat_ = addStatRow(form, tr("Enviado"), kSecondaryPurple);

        pages->addWidget(page);
    }

    // --- GPU ---
    {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(20, 16, 20, 16);
        gpuHeaderValue_ = buildHeader(layout, tr("GPU"), true);

        gpuGraph_ = new LineGraphWidget(page);
        gpuSeries_ = gpuGraph_->addSeries(tr("Uso"), kAccentBlue, 100.0, /*areaFill=*/true);
        layout->addWidget(gpuGraph_, 1);

        auto* form = buildStatsForm(layout);
        gpuVramStat_ = addStatRow(form, tr("VRAM"));

        pages->addWidget(page);
    }

    const QStringList navTitles = {tr("CPU"), tr("Memória"), tr("Disco"), tr("Rede"), tr("GPU")};
    QLabel** navValueSlots[] = {&cpuNavValue_, &memoryNavValue_, &diskNavValue_, &networkNavValue_, &gpuNavValue_};
    for (int i = 0; i < navTitles.size(); ++i) {
        auto* item = new QListWidgetItem();
        nav->addItem(item);
        QWidget* itemWidget = buildNavItem(navTitles[i], navValueSlots[i]);
        item->setSizeHint(QSize(nav->width(), 54));
        nav->setItemWidget(item, itemWidget);
    }

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
        const QString text = QString::fromStdString(util::formatPercent(percent));
        memoryNavValue_->setText(text);
        memoryHeaderValue_->setText(text);

        memoryUsedStat_->setText(QString::fromStdString(util::formatBytes(sample.usedKb() * 1024)));
        memoryAvailableStat_->setText(QString::fromStdString(util::formatBytes(sample.availableKb * 1024)));
        memoryTotalStat_->setText(QString::fromStdString(util::formatBytes(sample.totalKb * 1024)));
    });

    connect(monitor, &SystemMonitor::diskSampleReady, this, [this](const std::vector<DiskRate>& rates) {
        double totalRead = 0.0;
        double totalWrite = 0.0;
        for (const DiskRate& rate : rates) {
            totalRead += rate.readBytesPerSec;
            totalWrite += rate.writeBytesPerSec;
        }
        diskGraph_->addSample(diskReadSeries_, totalRead);
        diskGraph_->addSample(diskWriteSeries_, totalWrite);

        const QString readText = QString::fromStdString(util::formatBytes(static_cast<std::uint64_t>(totalRead))) + "/s";
        const QString writeText =
            QString::fromStdString(util::formatBytes(static_cast<std::uint64_t>(totalWrite))) + "/s";
        diskNavValue_->setText(readText);
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
