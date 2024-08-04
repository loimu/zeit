/* ========================================================================
*    Copyright (C) 2015-2024 Blaze <blaze@vivaldi.net>
*
*    This file is part of Zeit.
*
*    Zeit is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Zeit is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Zeit.  If not, see <http://www.gnu.org/licenses/>.
* ======================================================================== */

#include "config.h"

#include <unistd.h>
#include <QApplication>
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    if(getuid() == 0)
        qFatal("User should not be root");
    bool showTrayIcon = false;
    if(argc > 1) {
        QByteArray arg = argv[1];
        if(arg == QByteArray("-t") || arg == QByteArray("--tray-icon")) {
            showTrayIcon = true;
        }
    }
    QApplication a(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Zeit"));
    QApplication::setOrganizationName(QStringLiteral("zeit"));
    QApplication::setApplicationVersion(QStringLiteral(ZEIT_V));
    bool result;
    QTranslator translator;
    result = translator.load(QApplication::applicationDirPath() +
                             QLatin1String("/../share/zeit/translations/") +
                             QLatin1String("zeit_") +
                             QLocale::system().name() + QLatin1String(".qm"));
    a.installTranslator(&translator);
    QTranslator libTranslator;
    result &= libTranslator.load(QApplication::applicationDirPath() +
                                 QLatin1String("/../share/zeit/translations/") +
                                 QLatin1String("crontablib_") +
                                 QLocale::system().name() + QLatin1String(".qm"));
    a.installTranslator(&libTranslator);
    Q_UNUSED(result)
    MainWindow w;
    w.show();
    if(showTrayIcon)
        w.showTrayIcon();
    return a.exec();
}
