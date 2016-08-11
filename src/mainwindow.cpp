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
#include "ctvariable.h"
#include "ctInitializationError.h"

#include "aboutdialog.h"
#include "alarmdialog.h"
#include "listitem.h"
#include "taskdialog.h"
#include "timerdialog.h"
#include "variabledialog.h"
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
    ui->actionAddEntry->setIcon(QIcon::fromTheme(QLatin1String("document-new")));
    ui->actionModifyEntry->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    ui->actionDeleteEntry->setIcon(QIcon::fromTheme(QLatin1String("document-close")));
    ui->actionQuit->setIcon(QIcon::fromTheme(QLatin1String("application-exit")));
    ui->actionAlarm->setIcon(QIcon::fromTheme(QLatin1String("chronometer")));
    ui->actionTimer->setIcon(QIcon::fromTheme(QLatin1String("player-time")));
    ui->mainToolBar->addAction(ui->actionAddEntry);
    ui->mainToolBar->addAction(ui->actionModifyEntry);
    ui->mainToolBar->addAction(ui->actionDeleteEntry);
    ui->mainToolBar->addAction(ui->actionAlarm);
    ui->mainToolBar->addAction(ui->actionTimer);
    QActionGroup* group = new QActionGroup(this);
    ui->actionPeriodic->setActionGroup(group);
    ui->actionVariables->setActionGroup(group);
    ui->actionNonperiodic->setActionGroup(group);
    connect(ui->actionAddEntry, SIGNAL(triggered()), SLOT(createEntry()));
    connect(ui->actionModifyEntry, SIGNAL(triggered()), SLOT(modifyEntry()));
    connect(ui->actionDeleteEntry, SIGNAL(triggered()), SLOT(deleteEntry()));
    connect(ui->actionAlarm, SIGNAL(triggered()), SLOT(createAlarmDialog()));
    connect(ui->actionTimer, SIGNAL(triggered()), SLOT(createTimerDialog()));
    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(showAboutDialog()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(checkActions(QListWidgetItem*)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            SLOT(toggleStatus(QListWidgetItem*)));
    // init crontab host
    CTInitializationError error;
    ctHost = new CTHost(QLatin1String("crontab"), error);
    selectUser(false);
    // refresh state
    on_actionPeriodic_triggered();
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
    ui->actionAddEntry->setEnabled(currentUser);
    ui->actionModifyEntry->setEnabled(currentUser && enabled);
    ui->actionDeleteEntry->setEnabled(currentUser && enabled);
    ui->actionAlarm->setEnabled(currentUser);
    ui->actionTimer->setEnabled(currentUser);
}

void MainWindow::showTasks() {
    ui->listWidget->setEnabled(cron->isCurrentUserCron());
    ui->listWidget->clear();
    for(CTTask* ctTask: cron->tasks()) {
        ListItem* item = new ListItem(ctTask, ui->listWidget);
        ui->listWidget->addItem(item);
    }
}

void MainWindow::showVariables() {
    ui->listWidget->setEnabled(cron->isCurrentUserCron());
    ui->listWidget->clear();
    for(CTVariable* var : cron->variables()) {
        QListWidgetItem* item = new QListWidgetItem();
        QString comment = !var->comment.isEmpty() ? "#" + var->comment + "\n" : "";
        item->setText(comment + var->variable + "=" + var->value);
        if(var->enabled)
            item->setIcon(QIcon::fromTheme(QLatin1String("dialog-ok-apply")));
        else
            item->setIcon(QIcon::fromTheme(QLatin1String("edit-delete")));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::showNPTasks() {

}

void MainWindow::checkActions(QListWidgetItem* item) {
    refreshActions(item->isSelected());
    if(item->isSelected()) {
        ListItem* taskItem = static_cast<ListItem*>(item);
        currentTask = taskItem->task();
    }
}

void MainWindow::toggleStatus(QListWidgetItem* item) {
    int index = ui->listWidget->currentRow();
    if(index < 0)
        return;
    bool enabled = false;
    if(ui->actionPeriodic->isChecked()) {
        CTTask* task = cron->tasks().at(index);
        enabled = task->enabled = !task->enabled;
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(index);
        enabled = var->enabled = !var->enabled;
    }
    cron->save();
    if(enabled)
        item->setIcon(QIcon::fromTheme(QLatin1String("dialog-ok-apply")));
    else
        item->setIcon(QIcon::fromTheme(QLatin1String("edit-delete")));
}

void MainWindow::addTask() {
    cron->addTask(currentTask);
    cron->save();
    showTasks();
}

void MainWindow::modifyTask() {
    cron->modifyTask(currentTask);
    cron->save();
    showTasks();
}

void MainWindow::addVariable() {
    CTVariable* var = cron->variables().at(ui->listWidget->currentRow());
    cron->addVariable(var);
    cron->save();
    showVariables();
}

void MainWindow::modifyVariable() {
    CTVariable* var = cron->variables().at(ui->listWidget->currentRow());
    cron->modifyVariable(var);
    cron->save();
    showVariables();
}

void MainWindow::createEntry() {
    if(ui->actionPeriodic->isChecked()) {
        currentTask = new CTTask(QLatin1String(""),
                                 QLatin1String(""),
                                 cron->userLogin(),
                                 false);
        TaskDialog *td = new TaskDialog(currentTask, tr("New Task"), this);
        td->show();
        connect(td, SIGNAL(accepted()), SLOT(addTask()));
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = new CTVariable(QLatin1String(""),
                                         QLatin1String(""),
                                         cron->userLogin());
        VariableDialog* vd = new VariableDialog(var, tr("New Variable"), this);
        vd->show();
        connect(vd, SIGNAL(accepted()), SLOT(addVariable()));
    }
}

void MainWindow::deleteEntry() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Deleting Entry"), tr("Delete entry?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::No)
        return;
    if(ui->actionPeriodic->isChecked()) {
        cron->removeTask(currentTask);
        cron->save();
        showTasks();
        refreshActions(false);
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(ui->listWidget->currentRow());
        cron->removeVariable(var);
        cron->save();
        showVariables();
        refreshActions(false);
    }
}

void MainWindow::modifyEntry() {
    if(ui->actionPeriodic->isChecked()) {
        TaskDialog *td = new TaskDialog(currentTask, tr("Edit Task"), this);
        td->show();
        connect(td, SIGNAL(accepted()), SLOT(modifyTask()));
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(ui->listWidget->currentRow());
        VariableDialog *vd = new VariableDialog(var, tr("Edit Variable"), this);
        vd->show();
        connect(vd, SIGNAL(accepted()), SLOT(modifyVariable()));
    }
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
    if(ui->actionPeriodic->isChecked())
        showTasks();
    if(ui->actionVariables->isChecked())
        showVariables();
    refreshActions(false);
}

void MainWindow::on_actionPeriodic_triggered() {
    ui->actionAddEntry->setText(tr("New Task"));
    ui->actionModifyEntry->setText(tr("Modify Task"));
    ui->actionDeleteEntry->setText(tr("Delete Task"));
    ui->actionSystem->setEnabled(true);
    showTasks();
    refreshActions(false);
}

void MainWindow::on_actionVariables_triggered() {
    ui->actionAddEntry->setText(tr("New Variable"));
    ui->actionModifyEntry->setText(tr("Modify Variable"));
    ui->actionDeleteEntry->setText(tr("Delete Variable"));
    ui->actionSystem->setEnabled(true);
    showVariables();
    refreshActions(false);
}

void MainWindow::on_actionNonperiodic_triggered() {
    ui->actionAddEntry->setText(tr("New One-Shot Task"));
    ui->actionModifyEntry->setText(tr("Modify One-Shot Task"));
    ui->actionDeleteEntry->setText(tr("Delete One-Shot Task"));
    ui->listWidget->setEnabled(true);
    ui->listWidget->clear();
    selectUser(false);
    ui->actionSystem->setChecked(false);
    ui->actionSystem->setEnabled(false);
    showNPTasks();
    refreshActions(false);
}
