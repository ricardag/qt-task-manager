#include "ServicesView.h"

#include <QHeaderView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>

#include "../datasource/SystemdServices.h"
#include "../models/ServiceTableModel.h"

namespace taskmanager {

ServicesView::ServicesView(QWidget* parent) : QWidget(parent) {
    model_ = new ServiceTableModel(this);

    auto* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model_);

    auto* table = new QTableView(this);
    table->setModel(proxy);
    table->setSortingEnabled(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);

    auto* refreshButton = new QPushButton(tr("Atualizar"), this);
    connect(refreshButton, &QPushButton::clicked, this, &ServicesView::reload);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(refreshButton, 0, Qt::AlignRight);
    layout->addWidget(table);

    reload();
}

void ServicesView::reload() {
    model_->setServices(datasource::SystemdServices().listUnits());
}

} // namespace taskmanager
