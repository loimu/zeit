/* ========================================================================
*    Copyright (C) 2015-2019 Blaze <blaze@vivaldi.net>
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

    CTHost* ctHost;
    CTCron* cron;
    Ui::MainWindow *ui;
    Commands* commands;
    QAction* toggleItemAction;
    void refreshActions(bool);
    void showTasks();
    void showVariables();
    void showCommands();
    void selectUser(bool system);
    void refresh();
    void addTask(CTTask* task);
    /* Main menu */
    void addEntry();
    void modifyEntry();
    void deleteEntry();
    /* View menu */
    void viewTasks();
    void viewVariables();
    void viewCommands();
    /* Tools menu */
    void showAlarmDialog();
    void showTimerDialog();
    /* Help menu */
    void showAboutDialog();

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_H
