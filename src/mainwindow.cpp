/* ========================================================================
*    Copyright (C) 2015-2016 Blaze <blaze@vivaldi.net>
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
    currentTask = nullptr;
    ui->setupUi(this);
    ui->statusBar->hide();
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    // actions setup
    ui->actionAddTask->setIcon(QIcon::fromTheme(QLatin1String("document-new")));
    ui->actionModifyTask->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    ui->actionDeleteTask->setIcon(QIcon::fromTheme(QLatin1String("document-close")));
    ui->actionQuit->setIcon(QIcon::fromTheme(QLatin1String("application-exit")));
    ui->actionAlarm->setIcon(QIcon::fromTheme(QLatin1String("chronometer")));
    ui->actionTimer->setIcon(QIcon::fromTheme(QLatin1String("player-time")));
    ui->mainToolBar->addAction(ui->actionAddTask);
    ui->mainToolBar->addAction(ui->actionModifyTask);
    ui->mainToolBar->addAction(ui->actionDeleteTask);
    ui->mainToolBar->addAction(ui->actionAlarm);
    ui->mainToolBar->addAction(ui->actionTimer);
    connect(ui->actionAddTask, SIGNAL(triggered()), SLOT(createTaskDialog()));
    connect(ui->actionModifyTask, SIGNAL(triggered()), SLOT(modifyTaskDialog()));
    connect(ui->actionDeleteTask, SIGNAL(triggered()), SLOT(deleteTask()));
    connect(ui->actionAlarm, SIGNAL(triggered()), SLOT(createAlarmDialog()));
    connect(ui->actionTimer, SIGNAL(triggered()), SLOT(createTimerDialog()));
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(showAboutDialog()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(checkActions(QListWidgetItem*)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            SLOT(toggleTaskStatus(QListWidgetItem*)));
    // init crontab host
    CTInitializationError error;
    ctHost = new CTHost(QLatin1String("crontab"), error);
    selectUser(false);
    // refresh state
    refreshTasks();
    refreshActions(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectUser(bool system) {
    cron = system ? ctHost->findSystemCron() : ctHost->findCurrentUserCron();
}

void MainWindow::refreshActions(bool enabled) {
    bool currentUser = cron->isCurrentUserCron();
    ui->actionAddTask->setEnabled(currentUser);
    ui->actionModifyTask->setEnabled(currentUser && enabled);
    ui->actionDeleteTask->setEnabled(currentUser && enabled);
    ui->actionAlarm->setEnabled(currentUser);
}

void MainWindow::refreshTasks() {
    ui->listWidget->setEnabled(cron->isCurrentUserCron());
    ui->listWidget->clear();
    // Add new items
    ListItem* item;
    for(CTTask* ctTask: cron->tasks()) {
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
    reply = QMessageBox::question(this, tr("Deleting Task"), tr("Delete task?"),
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
    AlarmDialog *ad = new AlarmDialog(currentTask, this);
    ad->show();
    connect(ad, SIGNAL(accepted()), SLOT(addTask()));
}

void MainWindow::createTimerDialog() {
    TimerDialog *td = new TimerDialog(this);
    td->show();
}

void MainWindow::showAboutDialog() {
    ui->actionAbout->setDisabled(true);
    AboutDialog* about = new AboutDialog(this);
    about->show();
    connect(about, SIGNAL(destroyed(bool)), ui->actionAbout, SLOT(setEnabled(bool)));
}

void MainWindow::on_actionSystem_triggered(bool check) {
    selectUser(check);
    refreshTasks();
    refreshActions(false);
}

void MainWindow::on_actionPeriodic_triggered(bool check) {
    ui->actionSystem->setEnabled(true);
    ui->actionVariables->setChecked(false);
    ui->actionNonperiodic->setChecked(false);
    selectUser(false);
    refreshTasks();
    refreshActions(false);
    ui->actionPeriodic->setChecked(true);
}

void MainWindow::on_actionVariables_triggered(bool check) {
    ui->actionSystem->setChecked(false);
    ui->actionSystem->setEnabled(false);
    ui->actionPeriodic->setChecked(false);
    ui->actionNonperiodic->setChecked(false);
    ui->actionVariables->setChecked(true);
}

void MainWindow::on_actionNonperiodic_triggered(bool check) {
    ui->actionSystem->setChecked(false);
    ui->actionSystem->setEnabled(false);
    ui->actionPeriodic->setChecked(false);
    ui->actionVariables->setChecked(false);
    ui->actionNonperiodic->setChecked(true);
}
