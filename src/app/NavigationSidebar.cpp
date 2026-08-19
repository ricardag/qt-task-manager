#include "NavigationSidebar.h"

#include <QIcon>
#include <QListWidgetItem>

namespace taskmanager {

NavigationSidebar::NavigationSidebar(QWidget* parent) : QListWidget(parent) {
    setObjectName(QStringLiteral("navigationSidebar"));
    setFixedWidth(208);
    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(18, 18));
    setUniformItemSizes(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::NoFocus);

    static const struct {
        const char* icon;
        const char* label;
    } items[] = {
        {":/icons/processes.svg", QT_TR_NOOP("Processos")},
        {":/icons/performance.svg", QT_TR_NOOP("Desempenho")},
        {":/icons/app_history.svg", QT_TR_NOOP("Histórico de apps")},
        {":/icons/startup.svg", QT_TR_NOOP("Inicializar")},
        {":/icons/users.svg", QT_TR_NOOP("Usuários")},
        {":/icons/details.svg", QT_TR_NOOP("Detalhes")},
        {":/icons/services.svg", QT_TR_NOOP("Serviços")},
    };

    for (const auto& item : items) {
        addItem(new QListWidgetItem(QIcon(QString::fromUtf8(item.icon)), tr(item.label)));
    }
    setCurrentRow(0);
}

} // namespace taskmanager
