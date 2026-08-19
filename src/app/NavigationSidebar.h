#pragma once

#include <QListWidget>

namespace taskmanager {

// Left-hand navigation list styled after Windows 11's Task Manager
// NavigationView (icon + label rows, rounded selection highlight via QSS
// in resources/style.qss).
class NavigationSidebar : public QListWidget {
    Q_OBJECT

public:
    explicit NavigationSidebar(QWidget* parent = nullptr);
};

} // namespace taskmanager
