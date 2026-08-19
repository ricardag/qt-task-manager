#pragma once

#include <QAbstractTableModel>
#include <vector>

#include "../datasource/UserSessions.h"

namespace taskmanager {

class UserTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column { ColumnUser = 0, ColumnTerminal, ColumnHost, ColumnLoginTime, ColumnCount };

    explicit UserTableModel(QObject* parent = nullptr);

    void setSessions(std::vector<datasource::UserSession> sessions);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    std::vector<datasource::UserSession> rows_;
};

} // namespace taskmanager
