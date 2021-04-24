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

#include <QApplication>

#include "ctcron.h"
#include "cttask.h"
#include "../taskdialog.h"
#include "taskdelegate.h"
#include "ui_mainwindow.h"


TaskDelegate::TaskDelegate(Ui::MainWindow* ui, CTCron* cron_)
    : BaseDelegate(ui),
      cron(cron_)
{
    caption = tr("Task");
    toolTip = tr("crontab tasks, running periodically");
}

void TaskDelegate::view()
{
    ui->listWidget->setEnabled(cron->isCurrentUserCron() || ROOT_ACTIONS);
    ui->labelWarning->setVisible(ROOT_ACTIONS);
    ui->listWidget->clear();
    for(CTTask* task: cron->tasks()) {
        QListWidgetItem* item = new QListWidgetItem();
        QString text(tr("Description: %1\n"
                        "Runs %2\n"
                        "Command: %3",
                        "Runs at 'period described'")
                     .arg(task->comment, task->describe(), task->command));
        item->setText(text);
        setIcon(item, task->enabled);
        ui->listWidget->addItem(item);
    }
}

void TaskDelegate::createEntry() {
    CTTask* task = new CTTask(QString(), QString(),
                              cron->userLogin(), false);
    TaskDialog *td = new TaskDialog(task, tr("New Task"), ui->listWidget);
    td->show();
    QApplication::connect(td, &TaskDialog::accepted, td, [this, task] {
        cron->addTask(task);
        cron->save();
        if(ui->actionTasks->isChecked())
            view();
    });
}

void TaskDelegate::modifyEntry(int index) {
    CTTask* task = cron->tasks().at(index);
    TaskDialog* td = new TaskDialog(task, tr("Edit Task"), ui->listWidget);
    td->show();
    QApplication::connect(td, &TaskDialog::accepted, td, [this, task] {
        cron->modifyTask(task);
        cron->save();
        if(ui->actionTasks->isChecked())
            view();
    });
}

void TaskDelegate::deleteEntry(int index) {
    CTTask* task = cron->tasks().at(index);
    cron->removeTask(task);
    cron->save();
}

void TaskDelegate::toggleEntry(int index) {
    CTTask* task = cron->tasks().at(index);
    task->enabled = !task->enabled;
    cron->save();
    setIcon(ui->listWidget->item(index), task->enabled);
}
