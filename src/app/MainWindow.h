#pragma once

#include <QMainWindow>

namespace taskmanager {

class SystemMonitor;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    SystemMonitor* monitor_ = nullptr;
};

} // namespace taskmanager
