#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setOrganizationName("SmenaLTD.");
    QApplication::setApplicationName("Report");
    QApplication::setApplicationVersion("0.8.9");
    QApplication::setApplicationDisplayName("Report");
    MainWindow w;
    w.show();

    return a.exec();
}
