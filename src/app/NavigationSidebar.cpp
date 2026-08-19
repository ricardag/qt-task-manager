#include "NavigationSidebar.h"

namespace taskmanager {

NavigationSidebar::NavigationSidebar(QWidget* parent) : QListWidget(parent) {
    setObjectName(QStringLiteral("navigationSidebar"));
    setFixedWidth(200);
    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(20, 20));
    setUniformItemSizes(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    addItems({
        tr("Processos"),
        tr("Desempenho"),
        tr("Histórico de apps"),
        tr("Inicializar"),
        tr("Usuários"),
        tr("Detalhes"),
        tr("Serviços"),
    });
    setCurrentRow(0);
}

} // namespace taskmanager
