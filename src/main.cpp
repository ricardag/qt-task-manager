#include <QApplication>
#include <QFile>
#include <QFont>

#include "app/MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("TaskManager");
    QApplication::setOrganizationName("TaskManager");

    // Qt silently substitutes the nearest installed match when "Segoe UI
    // Variable"/"Segoe UI" aren't present, so this is safe on distros
    // without Microsoft fonts installed.
    QFont font(QStringLiteral("Segoe UI Variable Text"));
    font.setFamilies({QStringLiteral("Segoe UI Variable Text"), QStringLiteral("Segoe UI"),
                       QStringLiteral("Ubuntu"), QStringLiteral("Cantarell"), QStringLiteral("Noto Sans")});
    font.setPointSize(10);
    QApplication::setFont(font);

    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    taskmanager::MainWindow window;
    window.show();

    return QApplication::exec();
}
