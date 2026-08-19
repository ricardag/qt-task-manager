#pragma once

#include <QWidget>

namespace taskmanager {

class SystemMonitor;
class ProcessTableModel;

// Reuses ProcessTableModel like ProcessesView, but sorted by PID by default
// to match Task Manager's "Details" tab, which is process-centric rather
// than app-group-centric.
class DetailsView : public QWidget {
    Q_OBJECT

public:
    explicit DetailsView(SystemMonitor* monitor, QWidget* parent = nullptr);

private:
    ProcessTableModel* model_ = nullptr;
};

} // namespace taskmanager
