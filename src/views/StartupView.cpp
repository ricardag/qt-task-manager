#include "StartupView.h"

#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include "../models/StartupTableModel.h"

namespace taskmanager {

StartupView::StartupView(QWidget* parent) : QWidget(parent) {
    model_ = new StartupTableModel(this);

    auto* table = new QTableView(this);
    table->setModel(model_);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setFocusPolicy(Qt::NoFocus);
    table->horizontalHeader()->setSectionResizeMode(StartupTableModel::ColumnName, QHeaderView::Interactive);
    table->setColumnWidth(StartupTableModel::ColumnName, 200);
    table->horizontalHeader()->setSectionResizeMode(StartupTableModel::ColumnCommand, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(StartupTableModel::ColumnStatus, QHeaderView::ResizeToContents);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(30);

    auto* refreshButton = new QPushButton(tr("Atualizar"), this);
    connect(refreshButton, &QPushButton::clicked, this, &StartupView::reload);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(refreshButton, 0, Qt::AlignRight);
    layout->addWidget(table);

    reload();
}

void StartupView::reload() {
    model_->setEntries(loadStartupEntries());
}

} // namespace taskmanager
