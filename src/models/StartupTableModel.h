#pragma once

#include <QAbstractTableModel>
#include <vector>

namespace taskmanager {

struct StartupEntry {
    QString name;
    QString command;
    QString sourcePath; // the .desktop file this came from
    bool enabled = true;
};

// Scans XDG autostart directories (~/.config/autostart, /etc/xdg/autostart)
// for .desktop entries. This is Linux's closest equivalent to Windows'
// startup app list; there is no single canonical API for it.
std::vector<StartupEntry> loadStartupEntries();

class StartupTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column { ColumnName = 0, ColumnCommand, ColumnStatus, ColumnCount };

    explicit StartupTableModel(QObject* parent = nullptr);

    void setEntries(std::vector<StartupEntry> entries);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    std::vector<StartupEntry> rows_;
};

} // namespace taskmanager
