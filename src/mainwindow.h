/* ========================================================================
*    Copyright (C) 2015-2017 Blaze <blaze@vivaldi.net>
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

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    CTHost* ctHost;
    CTCron* cron;
    Ui::MainWindow *ui;
    Commands* commands;
    void refreshActions(bool);
    void showTasks();
    void showVariables();
    void showNPTasks();
    void selectUser(bool system);
    void refresh();
    void addTask(CTTask* task);
    void modifyTask(CTTask *task);
    void addVariable(CTVariable *var);
    void modifyVariable(CTVariable *var);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem*);
    void on_listWidget_itemDoubleClicked(QListWidgetItem*);
    // Zeit Menu
    void on_actionAddEntry_triggered();
    void on_actionModifyEntry_triggered();
    void on_actionDeleteEntry_triggered();
    // View Menu
    void on_actionRefresh_triggered();
    void on_actionSystem_triggered(bool);
    void on_actionPeriodic_triggered();
    void on_actionVariables_triggered();
    void on_actionNonperiodic_triggered();
    // Tools Menu
    void on_actionAlarm_triggered();
    void on_actionTimer_triggered();
    // Help Menu
    void on_actionAbout_triggered();
};

#endif // MAINWINDOW_H
