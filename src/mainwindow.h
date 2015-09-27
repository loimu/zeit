/* ========================================================================
*    Copyright (C) 2015 Blaze <blaze@open.by>
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
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    CTHost* ctHost;
    CTCron* cron;
    CTTask* currentTask;
    Ui::MainWindow *ui;
    /* Actions */
    QAction* createTaskAction;
    QAction* deleteTaskAction;
    QAction* modifyTaskAction;
    QAction* createAlarmAction;
    QAction* createTimerAction;
    void refreshActions(bool);
    void refreshTasks();

private slots:
    void checkActions(QListWidgetItem*);
    void addTask();
    void modifyTask();
    void showAboutDialog();
    void createTaskDialog();
    void deleteTask();
    void modifyTaskDialog();
    void createAlarmDialog();
    void createTimerDialog();
};

#endif // MAINWINDOW_H
