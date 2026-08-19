#include "UserTableModel.h"

#include <QDateTime>

namespace taskmanager {

using datasource::UserSession;

UserTableModel::UserTableModel(QObject* parent) : QAbstractTableModel(parent) {}

void UserTableModel::setSessions(std::vector<UserSession> sessions) {
    beginResetModel();
    rows_ = std::move(sessions);
    endResetModel();
}

int UserTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(rows_.size());
}

int UserTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant UserTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(rows_.size()) || role != Qt::DisplayRole) {
        return {};
    }

    const UserSession& session = rows_[index.row()];
    switch (index.column()) {
        case ColumnUser: return QString::fromStdString(session.username);
        case ColumnTerminal: return QString::fromStdString(session.terminal);
        case ColumnHost: return session.host.empty() ? QStringLiteral("local") : QString::fromStdString(session.host);
        case ColumnLoginTime:
            return QDateTime::fromSecsSinceEpoch(session.loginTimeEpoch).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
        default: return {};
    }
}

QVariant UserTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
        case ColumnUser: return QObject::tr("Usuário");
        case ColumnTerminal: return QObject::tr("Terminal");
        case ColumnHost: return QObject::tr("Host");
        case ColumnLoginTime: return QObject::tr("Login");
        default: return {};
    }
}

} // namespace taskmanager
