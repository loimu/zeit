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

#include <QMessageBox>

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"
#include "ctInitializationError.h"

#include "aboutdialog.h"
#include "alarmdialog.h"
#include "taskdialog.h"
#include "commanddialog.h"
#include "timerdialog.h"
#include "variabledialog.h"
#include "commands.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    commands(new Commands())
{
    ui->setupUi(this);
    ui->statusBar->hide();
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->actionAddEntry->setIcon(
                QIcon::fromTheme(QStringLiteral("document-new"),
                                 QIcon(QStringLiteral(
                                           ":/icons/document-new"))));
    ui->actionModifyEntry->setIcon(
                QIcon::fromTheme(QStringLiteral("document-edit"),
                                 QIcon(QStringLiteral(
                                           ":/icons/document-edit"))));
    ui->actionDeleteEntry->setIcon(
                QIcon::fromTheme(QStringLiteral("document-close"),
                                 QIcon(QStringLiteral(
                                           ":/icons/document-close"))));
    ui->actionQuit->setIcon(
                QIcon::fromTheme(QStringLiteral("application-exit"),
                                 QIcon(QStringLiteral(
                                           ":/icons/application-exit"))));
    ui->actionAlarm->setIcon(
                QIcon::fromTheme(QStringLiteral("chronometer"),
                                 QIcon(QStringLiteral(":/icons/chronometer"))));
    ui->actionTimer->setIcon(
                QIcon::fromTheme(QStringLiteral("player-time"),
                                 QIcon(QStringLiteral(":/icons/player-time"))));
    /* init crontab host */
    CTInitializationError error;
    ctHost = new CTHost(QStringLiteral("crontab"), error);
    selectUser(false);
    /* window actions */
    ui->mainToolBar->addAction(ui->actionAddEntry);
    ui->mainToolBar->addAction(ui->actionModifyEntry);
    ui->mainToolBar->addAction(ui->actionDeleteEntry);
    ui->mainToolBar->addAction(ui->actionAlarm);
    ui->mainToolBar->addAction(ui->actionTimer);
    QActionGroup* group = new QActionGroup(this);
    ui->actionTasks->setActionGroup(group);
    ui->actionVariables->setActionGroup(group);
    ui->actionCommands->setActionGroup(group);
    ui->listWidget->addAction(ui->actionModifyEntry);
    ui->listWidget->addAction(ui->actionDeleteEntry);
    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, [this] {
        refreshActions(ui->listWidget->currentItem()->isSelected());
    });
    connect(ui->listWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::toggleItem);
    // Main menu
    connect(ui->actionAddEntry, &QAction::triggered,
            this, &MainWindow::addEntry);
    connect(ui->actionModifyEntry, &QAction::triggered,
            this, &MainWindow::modifyEntry);
    connect(ui->actionDeleteEntry, &QAction::triggered,
            this, &MainWindow::deleteEntry);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    // View menu
    connect(ui->actionRefresh, &QAction::triggered, this, [=] {
        if(ui->actionCommands->isChecked())
            commands->refresh();
        refresh(); });
    connect(ui->actionSystem, &QAction::triggered, this, [=] (bool check) {
        selectUser(check);
        refresh(); });
    connect(ui->actionTasks, &QAction::triggered, this, &MainWindow::viewTasks);
    connect(ui->actionVariables, &QAction::triggered,
            this, &MainWindow::viewVariables);
    connect(ui->actionCommands, &QAction::triggered,
            this, &MainWindow::viewCommands);
    // Tools menu
    connect(ui->actionAlarm, &QAction::triggered,
            this, &MainWindow::showAlarmDialog);
    connect(ui->actionTimer, &QAction::triggered,
            this, &MainWindow::showTimerDialog);
    // Help menu
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::showAboutDialog);
    /* refresh state */
    viewTasks();
    refreshActions(false);
}

MainWindow::~MainWindow() {
    delete commands;
    delete ctHost;
    delete ui;
}

void MainWindow::refreshActions(bool enabled) {
    bool currentUser = cron->isCurrentUserCron();
    ui->actionAddEntry->setEnabled(currentUser);
    ui->actionModifyEntry->setEnabled((currentUser && enabled)
                                      && !ui->actionCommands->isChecked());
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
        text.append(QObject::tr("Description")
                    + QString(": %1\n").arg(task->comment));
        text.append(QObject::tr("Runs ", "Runs at 'period described'")
                    + task->describe());
        item->setText(text);
        if(task->enabled)
            item->setIcon(QIcon::fromTheme(
                              QStringLiteral("dialog-ok-apply"),
                              QIcon(
                                  QStringLiteral(":/icons/dialog-ok-apply"))));
        else
            item->setIcon(QIcon::fromTheme(
                              QStringLiteral("edit-delete"),
                              QIcon(QStringLiteral(":/icons/edit-delete"))));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::showVariables() {
    ui->listWidget->setEnabled(cron->isCurrentUserCron());
    ui->listWidget->clear();
    for(CTVariable* var : cron->variables()) {
        QListWidgetItem* item = new QListWidgetItem();
        QString comment = !var->comment.isEmpty() ? QLatin1String("## ")
                        + var->comment + QLatin1String("\n") : QString();
        item->setText(comment + var->variable
                      + QLatin1String("=") + var->value);
        if(var->enabled)
            item->setIcon(QIcon::fromTheme(
                              QStringLiteral("dialog-ok-apply"),
                              QIcon(
                                  QStringLiteral(":/icons/dialog-ok-apply"))));
        else
            item->setIcon(QIcon::fromTheme(
                              QStringLiteral("edit-delete"),
                              QIcon(QStringLiteral(":/icons/edit-delete"))));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::showCommands() {
    ui->listWidget->setEnabled(true);
    ui->listWidget->clear();
    for(Command& c : *commands->getCommands()) {
        QListWidgetItem* item = new QListWidgetItem(
                    c.description + QLatin1String("\nCommand: ") + c.command);
        ui->listWidget->addItem(item);
    }
}

void MainWindow::selectUser(bool system) {
    cron = system ? ctHost->findSystemCron() : ctHost->findCurrentUserCron();
}

void MainWindow::refresh() {
    if(ui->actionTasks->isChecked())
        showTasks();
    if(ui->actionVariables->isChecked())
        showVariables();
    if(ui->actionCommands->isChecked())
        showCommands();
    refreshActions(false);
}

void MainWindow::addTask(CTTask* task) {
    cron->addTask(task);
    cron->save();
    refresh();
}

void MainWindow::toggleItem(QListWidgetItem* item) {
    int index = ui->listWidget->currentRow();
    if(index < 0)
        return;
    bool enabled = false;
    if(ui->actionTasks->isChecked()) {
        CTTask* task = cron->tasks().at(index);
        enabled = task->enabled = !task->enabled;
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(index);
        enabled = var->enabled = !var->enabled;
    }
    cron->save();
    if(enabled)
        item->setIcon(QIcon::fromTheme(
                          QStringLiteral("dialog-ok-apply"),
                          QIcon(QStringLiteral(":/icons/dialog-ok-apply"))));
    else
        item->setIcon(QIcon::fromTheme(
                          QStringLiteral("edit-delete"),
                          QIcon(QStringLiteral(":/icons/edit-delete"))));
}

void MainWindow::addEntry() {
    if(ui->actionTasks->isChecked()) {
        CTTask* task = new CTTask(QString(), QString(),
                                  cron->userLogin(), false);
        TaskDialog *td = new TaskDialog(task, tr("New Task"), this);
        td->show();
        connect(td, &TaskDialog::accepted, this, &MainWindow::addTask);
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = new CTVariable(QString(),
                                         QString(), cron->userLogin());
        VariableDialog* vd = new VariableDialog(var, tr("New Variable"), this);
        vd->show();
        connect(vd, &VariableDialog::accepted, this, [=] {
            cron->addVariable(var);
            cron->save();
            refresh(); });
    }
    if(ui->actionCommands->isChecked()) {
        CommandDialog* cd = new CommandDialog(commands, this);
        cd->show();
        connect(cd, &CommandDialog::accepted, this, &MainWindow::refresh);
    }
}

void MainWindow::modifyEntry() {
    int index = ui->listWidget->currentRow();
    if(index < 0)
        return;
    if(ui->actionTasks->isChecked()) {
        CTTask* task = cron->tasks().at(index);
        TaskDialog* td = new TaskDialog(task, tr("Edit Task"), this);
        td->show();
        connect(td, &TaskDialog::accepted, this, [=] {
            cron->modifyTask(task);
            cron->save();
            refresh(); });
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(index);
        VariableDialog* vd = new VariableDialog(var, tr("Edit Variable"), this);
        vd->show();
        connect(vd, &VariableDialog::accepted, this, [=] {
            cron->modifyVariable(var);
            cron->save();
            refresh(); });
    }
}

void MainWindow::deleteEntry() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  tr("Deleting Entry"), tr("Delete entry?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::No)
        return;
    int index = ui->listWidget->currentRow();
    if(index < 0)
        return;
    if(ui->actionTasks->isChecked()) {
        CTTask* task = cron->tasks().at(index);
        cron->removeTask(task);
        cron->save();
    }
    if(ui->actionVariables->isChecked()) {
        CTVariable* var = cron->variables().at(index);
        cron->removeVariable(var);
        cron->save();
    }
    if(ui->actionCommands->isChecked())
        commands->deleteCommand(index);
    refresh();
}

void MainWindow::viewTasks() {
    ui->actionAddEntry->setText(tr("Add Task"));
    ui->actionModifyEntry->setText(tr("Modify Task"));
    ui->actionDeleteEntry->setText(tr("Delete Task"));
    // KDEBUG:391002 remove these three calls below once it's fixed
    ui->actionAddEntry->setIconText(tr("Add Task"));
    ui->actionModifyEntry->setIconText(tr("Modify Task"));
    ui->actionDeleteEntry->setIconText(tr("Delete Task"));
    ui->actionSystem->setEnabled(true);
    showTasks();
    refreshActions(false);
    ui->listWidget->setToolTip(tr("crontab tasks, running periodically"));
}

void MainWindow::viewVariables() {
    ui->actionAddEntry->setText(tr("Add Variable"));
    ui->actionModifyEntry->setText(tr("Modify Variable"));
    ui->actionDeleteEntry->setText(tr("Delete Variable"));
    // KDEBUG:391002 remove these three calls below once it's fixed
    ui->actionAddEntry->setIconText(tr("Add Variable"));
    ui->actionModifyEntry->setIconText(tr("Modify Variable"));
    ui->actionDeleteEntry->setIconText(tr("Delete Variable"));
    ui->actionSystem->setEnabled(true);
    showVariables();
    refreshActions(false);
    ui->listWidget->setToolTip(tr("environment variables for crontab"));
}

void MainWindow::viewCommands() {
    ui->actionAddEntry->setText(tr("Add Command"));
    ui->actionModifyEntry->setText(tr("Modify Command"));
    ui->actionDeleteEntry->setText(tr("Delete Command"));
    // KDEBUG:391002 remove these three calls below once it's fixed
    ui->actionAddEntry->setIconText(tr("Add Command"));
    ui->actionModifyEntry->setIconText(tr("Modify Command"));
    ui->actionDeleteEntry->setIconText(tr("Delete Command"));
    selectUser(false);
    ui->actionSystem->setChecked(false);
    ui->actionSystem->setEnabled(false);
    commands->refresh();
    showCommands();
    refreshActions(false);
    ui->listWidget->setToolTip(tr("commands, scheduled to be executed once"));
}

void MainWindow::showAlarmDialog() {
    CTTask* task = new CTTask(QString(), QString(), cron->userLogin(), false);
    AlarmDialog* ad = new AlarmDialog(task, this);
    ad->show();
    connect(ad, &AlarmDialog::accepted, this, &MainWindow::addTask);
}

void MainWindow::showTimerDialog() {
    TimerDialog* td = new TimerDialog(commands, this);
    td->show();
    connect(td, &TimerDialog::accepted, this, &MainWindow::refresh);
}

void MainWindow::showAboutDialog() {
    ui->actionAbout->setDisabled(true);
    AboutDialog* about = new AboutDialog(this);
    about->show();
    connect(about, &AboutDialog::destroyed,
            this, [this] { ui->actionAbout->setEnabled(true); });
}
