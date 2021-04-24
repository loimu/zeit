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

#include "config.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <QProcess>

#ifdef BUILD_HELPER
  #define ROOT_ACTIONS cron->isSystemCron()
#else
  #define ROOT_ACTIONS false
#endif // BUILD_HELPER

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"
#include "ctInitializationError.h"

#include "data/commanddelegate.h"
#include "data/taskdelegate.h"
#include "data/variabledelegate.h"
#include "aboutdialog.h"
#include "alarmdialog.h"
#include "taskdialog.h"
#include "commanddialog.h"
#include "timerdialog.h"
#include "variabledialog.h"
#include "commands.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define QSL QStringLiteral


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    commands(new Commands())
{
    ui->setupUi(this);
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->filterEdit->hide();
    ui->hideFilterButton->hide();
    ui->labelWarning->hide();
    ui->actionAddEntry->setIcon(QIcon::fromTheme(
                                    QStringLiteral("document-new"),
                                    QIcon(QSL(":/icons/document-new"))));
    ui->actionModifyEntry->setIcon(QIcon::fromTheme(
                                       QStringLiteral("document-edit"),
                                       QIcon(QSL(":/icons/document-edit"))));
    ui->actionDeleteEntry->setIcon(QIcon::fromTheme(
                                       QStringLiteral("document-close"),
                                       QIcon(QSL(":/icons/document-close"))));
    ui->actionQuit->setIcon(QIcon::fromTheme(
                                QStringLiteral("application-exit"),
                                QIcon(QSL(":/icons/application-exit"))));
    ui->actionAlarm->setIcon(QIcon::fromTheme(
                                 QStringLiteral("chronometer"),
                                 QIcon(QSL(":/icons/chronometer"))));
    ui->actionTimer->setIcon(QIcon::fromTheme(
                                 QStringLiteral("player-time"),
                                 QIcon(QSL(":/icons/player-time"))));
    /* init crontab host */
    CTInitializationError error;
    ctHost = new CTHost(QStringLiteral("crontab"), error);
    cron = ctHost->findCurrentUserCron();
    list = new TaskDelegate(ui, cron);
    /* check if `at` binary is available */
    QProcess proc;
    proc.start(QStringLiteral("which"), QStringList{QStringLiteral("at")});
    proc.waitForFinished(-1);
    ui->actionCommands->setDisabled(proc.readAllStandardOutput().isEmpty());
    /* window actions */
    ui->mainToolBar->addAction(ui->actionAddEntry);
    ui->mainToolBar->addAction(ui->actionModifyEntry);
    ui->mainToolBar->addAction(ui->actionDeleteEntry);
    ui->mainToolBar->addAction(ui->actionAlarm);
    ui->mainToolBar->addAction(ui->actionTimer);
    auto group = new QActionGroup(this);
    ui->actionTasks->setActionGroup(group);
    ui->actionVariables->setActionGroup(group);
    ui->actionCommands->setActionGroup(group);
    connect(ui->toggleItemAction, &QAction::triggered,
            this, [this] { list->toggleEntry(ui->listWidget->currentRow()); });
    ui->listWidget->addAction(ui->toggleItemAction);
    ui->listWidget->addAction(ui->actionModifyEntry);
    ui->listWidget->addAction(ui->actionDeleteEntry);
    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, [this] {
        refreshActions(ui->listWidget->currentRow() > -1
                       && ui->listWidget->currentItem()->isSelected());
    });
    connect(ui->listWidget, &QListWidget::itemDoubleClicked,
            ui->toggleItemAction, &QAction::trigger);
    connect(ui->hideFilterButton, &QPushButton::released,
            ui->actionShowFilter, &QAction::toggle);
    connect(ui->filterEdit, &QLineEdit::textEdited,
            this, [this] (const QString& text) {
        for(int i = 0; i < ui->listWidget->count(); i++) {
            QListWidgetItem* item = ui->listWidget->item(i);
            item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
        }
    });
    // Main menu
    connect(ui->actionAddEntry, &QAction::triggered,
            this, [this] { list->createEntry(); });
    connect(ui->actionModifyEntry, &QAction::triggered,
            this, [this] { list->modifyEntry(ui->listWidget->currentRow()); });
    connect(ui->actionDeleteEntry, &QAction::triggered,
            this, &MainWindow::deleteEntry);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    // View menu
    connect(ui->actionRefresh, &QAction::triggered, this, [this] {list->view();});
    connect(ui->actionSystem, &QAction::toggled, this, [this] (bool check) {
        cron = check ? ctHost->findSystemCron() : ctHost->findCurrentUserCron();
        switchView();
    });
    connect(group, &QActionGroup::triggered, this, &MainWindow::switchView);
    connect(ui->actionShowFilter, &QAction::toggled,
            this, &MainWindow::toggleFilter);
    connect(ui->actionWrapText, &QAction::toggled,
            ui->listWidget, &QListWidget::setWordWrap);
    // Tools menu
    connect(ui->actionAlarm, &QAction::triggered,
            this, &MainWindow::showAlarmDialog);
    connect(ui->actionTimer, &QAction::triggered,
            this, &MainWindow::showTimerDialog);
    // Help menu
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::showAboutDialog);
    updateWindow();
    refreshActions(false);
}

MainWindow::~MainWindow() {
    delete commands;
    delete ctHost;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
    if(e->key() == Qt::Key_Escape && ui->filterEdit->isVisible())
        ui->actionShowFilter->trigger();
}

void MainWindow::refreshActions(bool enabled) {
    ui->toggleItemAction->setDisabled(ui->actionCommands->isChecked() || !enabled);
    bool currentUser = cron->isCurrentUserCron() || cron->isSystemCron();
    ui->actionAddEntry->setEnabled(currentUser);
    ui->actionModifyEntry->setEnabled((currentUser && enabled)
                                      && !ui->actionCommands->isChecked());
    ui->actionDeleteEntry->setEnabled(currentUser && enabled);
    ui->actionAlarm->setEnabled(currentUser);
    ui->actionTimer->setEnabled(currentUser);
}

void MainWindow::updateWindow() {
    ui->actionAddEntry->setText(tr("Add ") + list->caption);
    ui->actionModifyEntry->setText(tr("Modify ") + list->caption);
    ui->actionDeleteEntry->setText(tr("Delete ") + list->caption);
    if(ui->actionCommands->isChecked())
        ui->actionSystem->setChecked(false);
    ui->actionSystem->setEnabled(!ui->actionCommands->isChecked());
    list->view();
    ui->listWidget->setToolTip(list->toolTip);
}

void MainWindow::switchView() {
    if(list) { delete list; list = nullptr; }
    if(ui->actionVariables->isChecked())
        list = new VariableDelegate(ui, cron);
    else if(ui->actionCommands->isChecked())
        list = new CommandDelegate(ui, commands);
    else
        list = new TaskDelegate(ui, cron);
    updateWindow();
}

void MainWindow::deleteEntry() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  tr("Deleting %1").arg(list->caption),
                                  tr("Delete %1?").arg(list->caption),
                                  QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::No)
        return;
    list->deleteEntry(ui->listWidget->currentRow());
    ui->actionRefresh->trigger();
}

void MainWindow::toggleFilter(bool check) {
    ui->filterEdit->setVisible(check);
    ui->hideFilterButton->setVisible(check);
    if(check) {
        ui->filterEdit->setFocus();
    } else {
        ui->filterEdit->clear();
        emit ui->filterEdit->textEdited({});
    }
}

void MainWindow::showAlarmDialog() {
    CTTask* task = new CTTask({}, {}, cron->userLogin(), false);
    AlarmDialog* ad = new AlarmDialog(task, this);
    ad->show();
    connect(ad, &AlarmDialog::accepted, this, [this, task] {
        cron->addTask(task);
        cron->save();
        if(ui->actionTasks->isChecked())
            list->view();
    });
}

void MainWindow::showTimerDialog() {
    TimerDialog* td = new TimerDialog(commands, this);
    td->show();
    connect(td, &TimerDialog::accepted,
            this, [this] { if(ui->actionCommands->isChecked()) list->view(); });
}

void MainWindow::showAboutDialog() {
    ui->actionAbout->setDisabled(true);
    AboutDialog* about = new AboutDialog(this);
    about->show();
    connect(about, &AboutDialog::destroyed,
            this, [this] { ui->actionAbout->setEnabled(true); });
}
