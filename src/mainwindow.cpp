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

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"
#include "ctInitializationError.h"

#include "aboutdialog.h"
#include "alarmdialog.h"
#include "listitem.h"
#include "taskdialog.h"
#include "timerdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    currentTask = NULL;
    ui->setupUi(this);
    ui->statusBar->hide();
    ui->mainToolBar->setMovable(false);
    // create ToolBar actions
    createTaskAction = new QAction(tr("New Task"), this);
    ui->mainToolBar->addAction(createTaskAction);
    deleteTaskAction = new QAction(tr("Delete Task"), this);
    ui->mainToolBar->addAction(deleteTaskAction);
    modifyTaskAction = new QAction(tr("Modify Task"), this);
    ui->mainToolBar->addAction(modifyTaskAction);
    createAlarmAction = new QAction(tr("New Alarm"), this);
    ui->mainToolBar->addAction(createAlarmAction);
    createTimerAction = new QAction(tr("New Timer"), this);
    ui->mainToolBar->addAction(createTimerAction);
    // connect MainMenu actions
    connect(ui->actionAddTask, SIGNAL(triggered()), SLOT(createTask()));
    connect(ui->actionModifyTask, SIGNAL(triggered()), SLOT(modifyTask()));
    connect(ui->actionDeleteTask, SIGNAL(triggered()), SLOT(deleteTask()));
    connect(ui->actionAlarm, SIGNAL(triggered()), SLOT(createAlarm()));
    connect(ui->actionTimer, SIGNAL(triggered()), SLOT(createTimer()));
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(showAboutDialog()));
    // connect ToolBar actions
    connect(createTaskAction, SIGNAL(triggered()), SLOT(createTask()));
    connect(modifyTaskAction, SIGNAL(triggered()), SLOT(modifyTask()));
    connect(deleteTaskAction, SIGNAL(triggered()), SLOT(deleteTask()));
    connect(createAlarmAction, SIGNAL(triggered()), SLOT(createAlarm()));
    connect(createTimerAction, SIGNAL(triggered()), SLOT(createTimer()));
    // connect ListWidget actions
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(checkActions(QListWidgetItem*)));
    CTInitializationError error;
    // init crontab host
    ctHost = new CTHost("crontab", error);
    // init cron instance
    cron = ctHost->findCurrentUserCron();
    // refresh state
    refreshTasks();
    refreshActions(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshActions(bool enabled) {
    deleteTaskAction->setEnabled(enabled);
    modifyTaskAction->setEnabled(enabled);
    ui->actionModifyTask->setEnabled(enabled);
    ui->actionDeleteTask->setEnabled(enabled);
}

void MainWindow::checkActions(QListWidgetItem* item) {
    refreshActions(item->isSelected());
    if(item->isSelected()) {
        ListItem* taskItem = static_cast<ListItem*>(item);
        currentTask = taskItem->task();
    }
}

void MainWindow::refreshTasks() {
    ui->listWidget->clear();
    // Add new items
    ListItem* item;
    foreach(CTTask* ctTask, cron->tasks()) {
        item = new ListItem(ctTask, ui->listWidget);
        ui->listWidget->addItem(item);
    }
}

void MainWindow::createTask() {
    currentTask = new CTTask(QLatin1String(""),
                             QLatin1String(""),
                             cron->userLogin(),
                             false);
    TaskDialog *td = new TaskDialog(currentTask, tr("New Task"), this);
    td->show();
    cron->addTask(currentTask);
    connect(td, SIGNAL(accepted()), SLOT(saveTask()));
}

void MainWindow::deleteTask() {
    cron->removeTask(currentTask);
    cron->save();
    refreshTasks();
}

void MainWindow::modifyTask() {
    TaskDialog *td = new TaskDialog(currentTask, tr("Edit Task"), this);
    td->show();
    cron->modifyTask(currentTask);
    connect(td, SIGNAL(accepted()), SLOT(saveTask()));
}

void MainWindow::saveTask() {
    cron->save();
    refreshTasks();
}

void MainWindow::createAlarm() {
    currentTask = new CTTask(QLatin1String(""),
                             QLatin1String(""),
                             cron->userLogin(),
                             false);
    AlarmDialog *ad = new AlarmDialog(currentTask, tr("New Alarm"), this);
    ad->show();
    cron->addTask(currentTask);
    connect(ad, SIGNAL(accepted()), SLOT(saveTask()));
}

void MainWindow::createTimer() {
    TimerDialog *td = new TimerDialog(tr("New Timer"), this);
    td->show();
}

void MainWindow::showAboutDialog() {
    ui->actionAbout->setDisabled(true);
    AboutDialog* about = new AboutDialog(this);
    about->show();
    connect(about, SIGNAL(destroyed(bool)), ui->actionAbout, SLOT(setEnabled(bool)));
}
