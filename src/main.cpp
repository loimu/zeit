#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Zeit");
    QApplication::setOrganizationName("zeit");
    QApplication::setApplicationVersion("0.1");

    QTranslator translator;
    translator.load(QLocale::system().name() + ".qm");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
