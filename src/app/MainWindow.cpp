#include "MainWindow.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QWidget>

#include "../SystemMonitor.h"
#include "../views/AppHistoryView.h"
#include "../views/DetailsView.h"
#include "../views/PerformanceView.h"
#include "../views/ProcessesView.h"
#include "../views/ServicesView.h"
#include "../views/StartupView.h"
#include "../views/UsersView.h"
#include "NavigationSidebar.h"

namespace taskmanager {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("Gerenciador de Tarefas"));
    resize(900, 600);

    monitor_ = new SystemMonitor(this);

    auto* sidebar = new NavigationSidebar(this);

    // Order must match NavigationSidebar's item order.
    auto* pages = new QStackedWidget(this);
    pages->addWidget(new ProcessesView(monitor_, this));
    pages->addWidget(new PerformanceView(monitor_, this));
    pages->addWidget(new AppHistoryView(this));
    pages->addWidget(new StartupView(this));
    pages->addWidget(new UsersView(this));
    pages->addWidget(new DetailsView(monitor_, this));
    pages->addWidget(new ServicesView(this));

    connect(sidebar, &QListWidget::currentRowChanged, pages, &QStackedWidget::setCurrentIndex);

    auto* central = new QWidget(this);
    central->setObjectName(QStringLiteral("centralWidget"));
    auto* layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(sidebar);
    layout->addWidget(pages, 1);
    setCentralWidget(central);

    monitor_->start(1000);
}

MainWindow::~MainWindow() = default;

} // namespace taskmanager
