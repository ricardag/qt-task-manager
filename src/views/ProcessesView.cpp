#include "ProcessesView.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>

#include "../SystemMonitor.h"
#include "../models/ProcessTableModel.h"

namespace taskmanager {

ProcessesView::ProcessesView(SystemMonitor* monitor, QWidget* parent) : QWidget(parent) {
    model_ = new ProcessTableModel(this);

    auto* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model_);
    proxy->setSortRole(Qt::DisplayRole);

    auto* table = new QTableView(this);
    table->setModel(proxy);
    table->setSortingEnabled(true);
    table->sortByColumn(ProcessTableModel::ColumnCpu, Qt::DescendingOrder);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(ProcessTableModel::ColumnName, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(table);

    connect(monitor, &SystemMonitor::processesUpdated, this,
            [this](const std::vector<ProcessSample>& processes) { model_->updateProcesses(processes); });
}

} // namespace taskmanager
