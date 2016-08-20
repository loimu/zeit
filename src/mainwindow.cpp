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
#include "taskdialog.h"
#include "timerdialog.h"
#include "variabledialog.h"
#include "commands.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    commands(new Commands())
{
    ui->setupUi(this);
    ui->statusBar->hide();
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    // actions setup
    ui->actionAddEntry->setIcon(
                QIcon::fromTheme(QStringLiteral("document-new"),
                                 QIcon(QStringLiteral(":/icons/document-new"))));
    ui->actionModifyEntry->setIcon(
                QIcon::fromTheme(QStringLiteral("document-edit"),
                                 QIcon(QStringLiteral(":/icons/document-edit"))));
    ui->actionDeleteEntry->setIcon(
                QIcon::fromTheme(QStringLiteral("document-close"),
                                 QIcon(QStringLiteral(":/icons/document-close"))));
    ui->actionQuit->setIcon(
                QIcon::fromTheme(QStringLiteral("application-exit"),
                                 QIcon(QStringLiteral(":/icons/application-exit"))));
    ui->actionAlarm->setIcon(
                QIcon::fromTheme(QStringLiteral("chronometer"),
                                 QIcon(QStringLiteral(":/icons/chronometer"))));
    ui->actionTimer->setIcon(
                QIcon::fromTheme(QStringLiteral("player-time"),
                                 QIcon(QStringLiteral(":/icons/player-time"))));
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
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            SLOT(checkActions(QListWidgetItem*)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            SLOT(toggleStatus(QListWidgetItem*)));
    // init crontab host
    CTInitializationError error;
    ctHost = new CTHost(QStringLiteral("crontab"), error);
    selectUser(false);
    // refresh state
    on_actionPeriodic_triggered();
    refreshActions(false);
}

MainWindow::~MainWindow()
{
    delete commands;
    delete ctHost;
    delete ui;
}

void MainWindow::selectUser(bool system) {
    cron = system ? ctHost->findSystemCron() : ctHost->findCurrentUserCron();
}

void MainWindow::refreshActions(bool enabled) {
    bool currentUser = cron->isCurrentUserCron();
    ui->actionAddEntry->setEnabled(currentUser);
    ui->actionModifyEntry->setEnabled((currentUser && enabled)
                                      && !ui->actionNonperiodic->isChecked());
    ui->actionDeleteEntry->setEnabled(currentUser && enabled);
    ui->actionAlarm->setEnabled(currentUser);
    ui->actionTimer->setEnabled(currentUser);
}

void MainWindow::showTasks() {
    ui->listWidget->setEnabled(cron->isCurrentUserCron());
    ui->listWidget->clear();
    for(CTTask* task: cron->tasks()) {
        QListWidgetItem* item = new QListWidgetItem();
        QString text;
        text.append(QObject::tr("Command: %1\n").arg(task->command));
        text.append(QObject::tr("Description") + QString(": %1\n").arg(task->comment));
        text.append(QObject::tr("Runs ", "Runs at 'period described'") + task->describe());
        item->setText(text);
        if(task->enabled)
            item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply"),
                                           QIcon(QStringLiteral(":/icons/dialog-ok-apply"))));
        else
            item->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete"),
                                           QIcon(QStringLiteral(":/icons/edit-delete"))));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::showVariables() {
    ui->listWidget->setEnabled(cron->isCurrentUserCron());
    ui->listWidget->clear();
    for(CTVariable* var : cron->variables()) {
        QListWidgetItem* item = new QListWidgetItem();
        QString comment = !var->comment.isEmpty() ? QLatin1String("#") + var->comment + QLatin1String("\n") : QLatin1String("");
        item->setText(comment + var->variable + QLatin1String("=") + var->value);
        if(var->enabled)
            item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply"),
                                           QIcon(QStringLiteral(":/icons/dialog-ok-apply"))));
        else
            item->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete"),
                                           QIcon(QStringLiteral(":/icons/edit-delete"))));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::showNPTasks() {
    ui->listWidget->setEnabled(true);
    ui->listWidget->clear();
    for(Command c : *commands->getCommands()) {
        QListWidgetItem* item = new QListWidgetItem(c.description +
                                                    QLatin1String("\nCommand: ")
                                                    + c.command);
        ui->listWidget->addItem(item);
    }
}

void MainWindow::refresh() {
    if(ui->actionPeriodic->isChecked())
        showTasks();
    if(ui->actionVariables->isChecked())
        showVariables();
    if(ui->actionNonperiodic->isChecked())
        showNPTasks();
    refreshActions(false);
}

void MainWindow::checkActions(QListWidgetItem* item) {
    refreshActions(item->isSelected());
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
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply"),
                                       QIcon(QStringLiteral(":/icons/dialog-ok-apply"))));
    else
        item->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete"),
                                       QIcon(QStringLiteral(":/icons/edit-delete"))));
}

void MainWindow::addTask(CTTask *task) {
    cron->addTask(task);
    cron->save();
    refresh();
}

void MainWindow::modifyTask(CTTask *task) {
    cron->modifyTask(task);
    cron->save();
    refresh();
}

void MainWindow::addVariable(CTVariable *var) {
    cron->addVariable(var);
    cron->save();
    refresh();
}

void MainWindow::modifyVariable(CTVariable* var) {
    cron->modifyVariable(var);
    cron->save();
    refresh();
}

void MainWindow::createEntry() {
    if(ui->actionPeriodic->isChecked()) {
        CTTask* task = new CTTask(QString(),QString(),cron->userLogin(),false);
        TaskDialog *td = new TaskDialog(task, tr("New Task"), this);
        td->show();
        connect(td, SIGNAL(accepted(CTTask*)), SLOT(addTask(CTTask*)));
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = new CTVariable(QString(),QString(),cron->userLogin());
        VariableDialog* vd = new VariableDialog(var, tr("New Variable"), this);
        vd->show();
        connect(vd,SIGNAL(accepted(CTVariable*)),SLOT(addVariable(CTVariable*)));
    }
}

void MainWindow::modifyEntry() {
    int index = ui->listWidget->currentRow();
    if(index < 0)
        return;
    if(ui->actionPeriodic->isChecked()) {
        CTTask* task = cron->tasks().at(index);
        TaskDialog *td = new TaskDialog(task, tr("Edit Task"), this);
        td->show();
        connect(td, SIGNAL(accepted(CTTask*)), SLOT(modifyTask(CTTask*)));
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(index);
        VariableDialog *vd = new VariableDialog(var, tr("Edit Variable"), this);
        vd->show();
        connect(vd, SIGNAL(accepted(CTVariable*)),
                SLOT(modifyVariable(CTVariable*)));
    }
}

void MainWindow::deleteEntry() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,tr("Deleting Entry"),tr("Delete entry?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::No)
        return;
    int index = ui->listWidget->currentRow();
    if(index < 0)
        return;
    if(ui->actionPeriodic->isChecked()) {
        CTTask* task = cron->tasks().at(index);
        cron->removeTask(task);
        cron->save();
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(index);
        cron->removeVariable(var);
        cron->save();
    }
    if(ui->actionNonperiodic->isChecked()) {
        commands->deleteCommand(index);
    }
    refresh();
}

void MainWindow::createAlarmDialog() {
    CTTask* task = new CTTask(QString(), QString(), cron->userLogin(), false);
    AlarmDialog *ad = new AlarmDialog(task, this);
    ad->show();
    connect(ad, SIGNAL(accepted(CTTask*)), SLOT(addTask(CTTask*)));
}

void MainWindow::createTimerDialog() {
    TimerDialog *td = new TimerDialog(commands, this);
    td->show();
    connect(td, SIGNAL(accepted()), SLOT(refresh()));
}

void MainWindow::showAboutDialog() {
    ui->actionAbout->setDisabled(true);
    AboutDialog* about = new AboutDialog(this);
    about->show();
    connect(about, SIGNAL(destroyed(bool)),
            ui->actionAbout,SLOT(setEnabled(bool)));
}

void MainWindow::on_actionSystem_triggered(bool check) {
    selectUser(check);
    refresh();
}

void MainWindow::on_actionPeriodic_triggered() {
    ui->actionAddEntry->setText(tr("Add Task"));
    ui->actionModifyEntry->setText(tr("Modify Task"));
    ui->actionDeleteEntry->setText(tr("Delete Task"));
    ui->actionSystem->setEnabled(true);
    showTasks();
    refreshActions(false);
}

void MainWindow::on_actionVariables_triggered() {
    ui->actionAddEntry->setText(tr("Add Variable"));
    ui->actionModifyEntry->setText(tr("Modify Variable"));
    ui->actionDeleteEntry->setText(tr("Delete Variable"));
    ui->actionSystem->setEnabled(true);
    showVariables();
    refreshActions(false);
}

void MainWindow::on_actionNonperiodic_triggered() {
    ui->actionAddEntry->setText(tr("Add Command"));
    ui->actionModifyEntry->setText(tr("Modify Command"));
    ui->actionDeleteEntry->setText(tr("Delete Command"));
    selectUser(false);
    ui->actionSystem->setChecked(false);
    ui->actionSystem->setEnabled(false);
    commands->refresh();
    showNPTasks();
    refreshActions(false);
}

void MainWindow::on_actionRefresh_triggered() {
    if(ui->actionNonperiodic->isChecked())
        commands->refresh();
    refresh();
}
