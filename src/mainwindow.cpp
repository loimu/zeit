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

#include <QMessageBox>

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
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    // main menu actions
    ui->actionAddTask->setIcon(QIcon::fromTheme(QLatin1String("document-new")));
    ui->actionModifyTask->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    ui->actionDeleteTask->setIcon(QIcon::fromTheme(QLatin1String("document-close")));
    ui->actionQuit->setIcon(QIcon::fromTheme(QLatin1String("application-exit")));
    ui->actionAlarm->setIcon(QIcon::fromTheme(QLatin1String("chronometer")));
    ui->actionTimer->setIcon(QIcon::fromTheme(QLatin1String("player-time")));
    // create ToolBar actions
    createTaskAction = new QAction(tr("New Task"), this);
    createTaskAction->setIcon(QIcon::fromTheme(QLatin1String("document-new")));
    ui->mainToolBar->addAction(createTaskAction);
    deleteTaskAction = new QAction(tr("Delete Task"), this);
    deleteTaskAction->setIcon(QIcon::fromTheme(QLatin1String("document-close")));
    ui->mainToolBar->addAction(deleteTaskAction);
    modifyTaskAction = new QAction(tr("Modify Task"), this);
    modifyTaskAction->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    ui->mainToolBar->addAction(modifyTaskAction);
    createAlarmAction = new QAction(tr("New Alarm"), this);
    createAlarmAction->setIcon(QIcon::fromTheme(QLatin1String("chronometer")));
    ui->mainToolBar->addAction(createAlarmAction);
    createTimerAction = new QAction(tr("New Timer"), this);
    createTimerAction->setIcon(QIcon::fromTheme(QLatin1String("player-time")));
    ui->mainToolBar->addAction(createTimerAction);
    // connect MainMenu actions
    connect(ui->actionAddTask, SIGNAL(triggered()), SLOT(createTaskDialog()));
    connect(ui->actionModifyTask, SIGNAL(triggered()), SLOT(modifyTaskDialog()));
    connect(ui->actionDeleteTask, SIGNAL(triggered()), SLOT(deleteTask()));
    connect(ui->actionAlarm, SIGNAL(triggered()), SLOT(createAlarmDialog()));
    connect(ui->actionTimer, SIGNAL(triggered()), SLOT(createTimerDialog()));
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(showAboutDialog()));
    // connect ToolBar actions
    connect(createTaskAction, SIGNAL(triggered()), SLOT(createTaskDialog()));
    connect(modifyTaskAction, SIGNAL(triggered()), SLOT(modifyTaskDialog()));
    connect(deleteTaskAction, SIGNAL(triggered()), SLOT(deleteTask()));
    connect(createAlarmAction, SIGNAL(triggered()), SLOT(createAlarmDialog()));
    connect(createTimerAction, SIGNAL(triggered()), SLOT(createTimerDialog()));
    // connect ListWidget actions
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(checkActions(QListWidgetItem*)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            SLOT(toggleTaskStatus(QListWidgetItem*)));
    // init crontab host
    CTInitializationError error;
    ctHost = new CTHost(QLatin1String("crontab"), error);
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

void MainWindow::refreshTasks() {
    ui->listWidget->clear();
    // Add new items
    ListItem* item;
    foreach(CTTask* ctTask, cron->tasks()) {
        item = new ListItem(ctTask, ui->listWidget);
        ui->listWidget->addItem(item);
    }
}

void MainWindow::checkActions(QListWidgetItem* item) {
    refreshActions(item->isSelected());
    if(item->isSelected()) {
        ListItem* taskItem = static_cast<ListItem*>(item);
        currentTask = taskItem->task();
    }
}

void MainWindow::toggleTaskStatus(QListWidgetItem* item) {
    ListItem* taskItem = static_cast<ListItem*>(item);
    taskItem->toggleStatus();
    cron->save();
}

void MainWindow::addTask() {
    cron->addTask(currentTask);
    cron->save();
    refreshTasks();
}


void MainWindow::modifyTask() {
    cron->modifyTask(currentTask);
    cron->save();
    refreshTasks();
}

void MainWindow::createTaskDialog() {
    currentTask = new CTTask(QLatin1String(""),
                             QLatin1String(""),
                             cron->userLogin(),
                             false);
    TaskDialog *td = new TaskDialog(currentTask, tr("New Task"), this);
    td->show();
    connect(td, SIGNAL(accepted()), SLOT(addTask()));
}

void MainWindow::deleteTask() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Deleting Task", "Delete task?",
                                  QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::No)
        return;
    cron->removeTask(currentTask);
    cron->save();
    refreshTasks();
    refreshActions(false);
}

void MainWindow::modifyTaskDialog() {
    TaskDialog *td = new TaskDialog(currentTask, tr("Edit Task"), this);
    td->show();
    connect(td, SIGNAL(accepted()), SLOT(modifyTask()));
}

void MainWindow::createAlarmDialog() {
    currentTask = new CTTask(QLatin1String(""),
                             QLatin1String(""),
                             cron->userLogin(),
                             false);
    AlarmDialog *ad = new AlarmDialog(currentTask, tr("New Alarm"), this);
    ad->show();
    connect(ad, SIGNAL(accepted()), SLOT(addTask()));
}

void MainWindow::createTimerDialog() {
    TimerDialog *td = new TimerDialog(tr("New Timer"), this);
    td->show();
}

void MainWindow::showAboutDialog() {
    ui->actionAbout->setDisabled(true);
    AboutDialog* about = new AboutDialog(this);
    about->show();
    connect(about, SIGNAL(destroyed(bool)), ui->actionAbout, SLOT(setEnabled(bool)));
}
