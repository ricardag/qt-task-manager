#pragma once

#include <QAbstractTableModel>
#include <vector>

#include "../datasource/SystemdServices.h"

namespace taskmanager {

class ServiceTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column { ColumnName = 0, ColumnDescription, ColumnStatus, ColumnCount };

    explicit ServiceTableModel(QObject* parent = nullptr);

    void setServices(std::vector<datasource::ServiceUnit> services);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    std::vector<datasource::ServiceUnit> rows_;
};

} // namespace taskmanager
