#pragma once

#include <QWidget>

namespace taskmanager {

class SystemMonitor;
class ProcessTableModel;

class ProcessesView : public QWidget {
    Q_OBJECT

public:
    explicit ProcessesView(SystemMonitor* monitor, QWidget* parent = nullptr);

private:
    ProcessTableModel* model_ = nullptr;
};

} // namespace taskmanager
