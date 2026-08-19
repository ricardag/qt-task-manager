#pragma once

#include <QAbstractTableModel>
#include <vector>

#include "../SystemMonitor.h"

namespace taskmanager {

// Table model for the Processes view. update() diffs incoming samples
// against the current rows by PID instead of resetting the whole model,
// so QTableView keeps the user's selection and scroll position stable
// across ticks the way the real Task Manager does.
class ProcessTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        ColumnName = 0,
        ColumnPid,
        ColumnUser,
        ColumnCpu,
        ColumnMemory,
        ColumnCount,
    };

    explicit ProcessTableModel(QObject* parent = nullptr);

    void updateProcesses(std::vector<ProcessSample> processes);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    std::vector<ProcessSample> rows_;
};

} // namespace taskmanager
