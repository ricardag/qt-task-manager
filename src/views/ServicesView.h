#pragma once

#include <QWidget>

namespace taskmanager {

class ServiceTableModel;

class ServicesView : public QWidget {
    Q_OBJECT

public:
    explicit ServicesView(QWidget* parent = nullptr);

private:
    void reload();

    ServiceTableModel* model_ = nullptr;
};

} // namespace taskmanager
