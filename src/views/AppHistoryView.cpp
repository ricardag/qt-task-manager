#include "AppHistoryView.h"

#include <QLabel>
#include <QVBoxLayout>

namespace taskmanager {

AppHistoryView::AppHistoryView(QWidget* parent) : QWidget(parent) {
    auto* label = new QLabel(
        tr("O Linux não mantém um histórico de uso de recursos por aplicativo como o Windows.\n"
           "Este recurso será revisitado em uma versão futura (ex.: métricas acumuladas desde o boot)."),
        this);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignCenter);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(label);
}

} // namespace taskmanager
