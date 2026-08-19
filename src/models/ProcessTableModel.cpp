#include "ProcessTableModel.h"

#include <unordered_map>

#include "../util/Formatting.h"

namespace taskmanager {

ProcessTableModel::ProcessTableModel(QObject* parent) : QAbstractTableModel(parent) {}

void ProcessTableModel::updateProcesses(std::vector<ProcessSample> processes) {
    std::unordered_map<int, int> newIndexByPid;
    newIndexByPid.reserve(processes.size());
    for (int i = 0; i < static_cast<int>(processes.size()); ++i) {
        newIndexByPid[processes[i].info.pid] = i;
    }

    // Remove rows for processes that exited, back-to-front so indices stay valid.
    for (int row = static_cast<int>(rows_.size()) - 1; row >= 0; --row) {
        if (newIndexByPid.find(rows_[row].info.pid) == newIndexByPid.end()) {
            beginRemoveRows(QModelIndex(), row, row);
            rows_.erase(rows_.begin() + row);
            endRemoveRows();
        }
    }

    std::unordered_map<int, int> oldIndexByPid;
    oldIndexByPid.reserve(rows_.size());
    for (int i = 0; i < static_cast<int>(rows_.size()); ++i) {
        oldIndexByPid[rows_[i].info.pid] = i;
    }

    for (const ProcessSample& sample : processes) {
        const auto it = oldIndexByPid.find(sample.info.pid);
        if (it != oldIndexByPid.end()) {
            rows_[it->second] = sample;
            emit dataChanged(index(it->second, 0), index(it->second, ColumnCount - 1));
        }
    }

    for (const ProcessSample& sample : processes) {
        if (oldIndexByPid.find(sample.info.pid) == oldIndexByPid.end()) {
            const int row = static_cast<int>(rows_.size());
            beginInsertRows(QModelIndex(), row, row);
            rows_.push_back(sample);
            endInsertRows();
        }
    }
}

int ProcessTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(rows_.size());
}

int ProcessTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant ProcessTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(rows_.size())) {
        return {};
    }

    const ProcessSample& sample = rows_[index.row()];

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case ColumnPid:
            case ColumnCpu:
            case ColumnMemory:
                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
            default:
                return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (index.column()) {
        case ColumnName:
            return QString::fromStdString(sample.info.name);
        case ColumnPid:
            return sample.info.pid;
        case ColumnUser:
            return sample.info.uid >= 0 ? QString::number(sample.info.uid) : QStringLiteral("-");
        case ColumnCpu:
            return QString::fromStdString(util::formatPercent(sample.cpuPercent));
        case ColumnMemory:
            return QString::fromStdString(util::formatBytes(sample.info.vmRssKb * 1024));
        default:
            return {};
    }
}

QVariant ProcessTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch (section) {
        case ColumnName: return QObject::tr("Nome");
        case ColumnPid: return QObject::tr("PID");
        case ColumnUser: return QObject::tr("Usuário");
        case ColumnCpu: return QObject::tr("CPU");
        case ColumnMemory: return QObject::tr("Memória");
        default: return {};
    }
}

} // namespace taskmanager
