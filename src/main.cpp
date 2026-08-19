#include <QApplication>
#include <QFile>

#include "app/MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("TaskManager");
    QApplication::setOrganizationName("TaskManager");

    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    taskmanager::MainWindow window;
    window.show();

    return QApplication::exec();
}
