#pragma once

#include <QWidget>

namespace taskmanager {

class UserTableModel;

class UsersView : public QWidget {
    Q_OBJECT

public:
    explicit UsersView(QWidget* parent = nullptr);

private:
    void reload();

    UserTableModel* model_ = nullptr;
};

} // namespace taskmanager
