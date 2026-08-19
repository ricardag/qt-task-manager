#include "PerformanceView.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

#include "../SystemMonitor.h"
#include "LineGraphWidget.h"

namespace taskmanager {

namespace {
constexpr int kFirstGpuIndex = 0;
} // namespace

PerformanceView::PerformanceView(SystemMonitor* monitor, QWidget* parent) : QWidget(parent) {
    auto* nav = new QListWidget(this);
    nav->setFixedWidth(160);
    nav->addItems({tr("CPU"), tr("Memória"), tr("Disco"), tr("Rede"), tr("GPU")});

    auto* pages = new QStackedWidget(this);

    cpuGraph_ = new LineGraphWidget(tr("CPU"), this);
    cpuSeries_ = cpuGraph_->addSeries(tr("Uso total"), 100.0);
    pages->addWidget(cpuGraph_);

    memoryGraph_ = new LineGraphWidget(tr("Memória"), this);
    memorySeries_ = memoryGraph_->addSeries(tr("Em uso"), 100.0);
    pages->addWidget(memoryGraph_);

    diskGraph_ = new LineGraphWidget(tr("Disco"), this);
    diskReadSeries_ = diskGraph_->addSeries(tr("Leitura (B/s)"));
    diskWriteSeries_ = diskGraph_->addSeries(tr("Escrita (B/s)"));
    pages->addWidget(diskGraph_);

    networkGraph_ = new LineGraphWidget(tr("Rede"), this);
    networkRxSeries_ = networkGraph_->addSeries(tr("Recebido (B/s)"));
    networkTxSeries_ = networkGraph_->addSeries(tr("Enviado (B/s)"));
    pages->addWidget(networkGraph_);

    gpuGraph_ = new LineGraphWidget(tr("GPU"), this);
    gpuSeries_ = gpuGraph_->addSeries(tr("Uso"), 100.0);
    pages->addWidget(gpuGraph_);

    connect(nav, &QListWidget::currentRowChanged, pages, &QStackedWidget::setCurrentIndex);
    nav->setCurrentRow(0);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(nav);
    layout->addWidget(pages, 1);

    connect(monitor, &SystemMonitor::cpuSampleReady, this,
            [this](double totalPercent, const std::vector<double>&) {
                cpuGraph_->addSample(cpuSeries_, totalPercent);
            });

    connect(monitor, &SystemMonitor::memorySampleReady, this, [this](const datasource::MemorySample& sample) {
        const double percent = sample.totalKb > 0
                                    ? (static_cast<double>(sample.usedKb()) / static_cast<double>(sample.totalKb)) * 100.0
                                    : 0.0;
        memoryGraph_->addSample(memorySeries_, percent);
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
    });

    connect(monitor, &SystemMonitor::gpuSampleReady, this, [this](const std::vector<datasource::GpuSample>& samples) {
        const double usage = (!samples.empty() && samples[kFirstGpuIndex].usagePercent >= 0)
                                  ? static_cast<double>(samples[kFirstGpuIndex].usagePercent)
                                  : 0.0;
        gpuGraph_->addSample(gpuSeries_, usage);
    });
}

} // namespace taskmanager
