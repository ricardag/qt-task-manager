#pragma once

#include <QWidget>

namespace taskmanager {

// Linux has no equivalent to Windows' per-app resource history log; see
// README for the product decision behind this placeholder.
class AppHistoryView : public QWidget {
    Q_OBJECT

public:
    explicit AppHistoryView(QWidget* parent = nullptr);
};

} // namespace taskmanager
