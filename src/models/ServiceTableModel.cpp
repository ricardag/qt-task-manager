#include "ServiceTableModel.h"

namespace taskmanager {

using datasource::ServiceUnit;

ServiceTableModel::ServiceTableModel(QObject* parent) : QAbstractTableModel(parent) {}

void ServiceTableModel::setServices(std::vector<ServiceUnit> services) {
    beginResetModel();
    rows_ = std::move(services);
    endResetModel();
}

int ServiceTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(rows_.size());
}

int ServiceTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant ServiceTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(rows_.size()) || role != Qt::DisplayRole) {
        return {};
    }

    const ServiceUnit& unit = rows_[index.row()];
    switch (index.column()) {
        case ColumnName: return unit.name;
        case ColumnDescription: return unit.description;
        case ColumnStatus: return unit.activeState + QStringLiteral(" (") + unit.subState + QStringLiteral(")");
        default: return {};
    }
}

QVariant ServiceTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
        case ColumnName: return QObject::tr("Nome");
        case ColumnDescription: return QObject::tr("Descrição");
        case ColumnStatus: return QObject::tr("Status");
        default: return {};
    }
}

} // namespace taskmanager
