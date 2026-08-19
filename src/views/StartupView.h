#pragma once

#include <QWidget>

namespace taskmanager {

class StartupTableModel;

class StartupView : public QWidget {
    Q_OBJECT

public:
    explicit StartupView(QWidget* parent = nullptr);

private:
    void reload();

    StartupTableModel* model_ = nullptr;
};

} // namespace taskmanager
