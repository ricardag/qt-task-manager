#include "UsersView.h"

#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

#include "../datasource/UserSessions.h"
#include "../models/UserTableModel.h"

namespace taskmanager {

UsersView::UsersView(QWidget* parent) : QWidget(parent) {
    model_ = new UserTableModel(this);

    auto* table = new QTableView(this);
    table->setModel(model_);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(table);

    reload();

    // Logged-in users change rarely; a slow periodic refresh is enough,
    // unlike the per-second cadence SystemMonitor uses for process/CPU data.
    auto* refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &UsersView::reload);
    refreshTimer->start(10000);
}

void UsersView::reload() {
    model_->setSessions(datasource::UserSessions().listSessions());
}

} // namespace taskmanager
