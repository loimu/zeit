/* ========================================================================
*    Copyright (C) 2015-2021 Blaze <blaze@vivaldi.net>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CTHost;
class CTCron;
class CTTask;
class CTVariable;
class QListWidgetItem;
class Commands;
class BaseDelegate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    CTHost* ctHost = nullptr;
    CTCron* cron = nullptr;
    Ui::MainWindow *ui;
    Commands* commands;
    BaseDelegate* list = nullptr;
    void keyPressEvent(QKeyEvent*);
    void refreshActions(bool);
    void updateWindow();
    void switchView();
    void deleteEntry();
    void toggleFilter(bool check);
    void showAlarmDialog();
    void showTimerDialog();
    void showAboutDialog();

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_H
