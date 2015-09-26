#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QApplication::setApplicationName("Zeit");
    QApplication::setOrganizationName("zeit");
    QApplication::setApplicationVersion("0.1");

    return a.exec();
}
