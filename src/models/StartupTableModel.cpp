#include "StartupTableModel.h"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

namespace taskmanager {

namespace {

void scanDirectory(const QString& dirPath, std::vector<StartupEntry>& out) {
    QDir dir(dirPath);
    if (!dir.exists()) return;

    const QStringList desktopFiles = dir.entryList({QStringLiteral("*.desktop")}, QDir::Files);
    for (const QString& fileName : desktopFiles) {
        const QString fullPath = dir.filePath(fileName);
        QSettings settings(fullPath, QSettings::IniFormat);
        settings.beginGroup(QStringLiteral("Desktop Entry"));

        StartupEntry entry;
        entry.name = settings.value(QStringLiteral("Name"), fileName).toString();
        entry.command = settings.value(QStringLiteral("Exec")).toString();
        entry.sourcePath = fullPath;

        const bool hidden = settings.value(QStringLiteral("Hidden"), false).toBool();
        const bool gnomeEnabled = settings.value(QStringLiteral("X-GNOME-Autostart-enabled"), true).toBool();
        entry.enabled = !hidden && gnomeEnabled;

        settings.endGroup();
        out.push_back(std::move(entry));
    }
}

} // namespace

std::vector<StartupEntry> loadStartupEntries() {
    std::vector<StartupEntry> entries;
    scanDirectory(QStringLiteral("/etc/xdg/autostart"), entries);
    scanDirectory(QDir::homePath() + QStringLiteral("/.config/autostart"), entries);
    return entries;
}

StartupTableModel::StartupTableModel(QObject* parent) : QAbstractTableModel(parent) {}

void StartupTableModel::setEntries(std::vector<StartupEntry> entries) {
    beginResetModel();
    rows_ = std::move(entries);
    endResetModel();
}

int StartupTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(rows_.size());
}

int StartupTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant StartupTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(rows_.size()) || role != Qt::DisplayRole) {
        return {};
    }

    const StartupEntry& entry = rows_[index.row()];
    switch (index.column()) {
        case ColumnName: return entry.name;
        case ColumnCommand: return entry.command;
        case ColumnStatus: return entry.enabled ? QObject::tr("Habilitado") : QObject::tr("Desabilitado");
        default: return {};
    }
}

QVariant StartupTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
        case ColumnName: return QObject::tr("Nome");
        case ColumnCommand: return QObject::tr("Comando");
        case ColumnStatus: return QObject::tr("Status");
        default: return {};
    }
}

} // namespace taskmanager
