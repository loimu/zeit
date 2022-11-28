/* ========================================================================
*    Copyright (C) 2015-2022 Blaze <blaze@vivaldi.net>
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
#include <QListWidget>

#include "ctcron.h"
#include "cttask.h"
#include "taskdialog.h"
#include "taskdelegate.h"


TaskDelegate::TaskDelegate(QListWidget* widget, CTCron* cron_)
    : BaseDelegate(widget),
      cron(cron_)
{
    caption = tr("Task");
    toolTip = tr("crontab tasks, running periodically");
}

void TaskDelegate::view() {
    widget->setEnabled(cron->isCurrentUserCron() || ROOT_ACTIONS);
    widget->clear();
    for(CTTask* task: cron->tasks()) {
        QListWidgetItem* item = new QListWidgetItem();
        setIcon(item, task->enabled);
        QString text(QStringLiteral("%1\n%2\n%3").arg(
                         elideText(tr("Description: ") + task->comment),
                         elideText(tr("Runs ", "Runs at 'period described'")
                                   + task->describe()),
                         elideText(tr("Command: ") + task->command)));
        item->setText(text);
        widget->addItem(item);
    }
}

void TaskDelegate::copyEntry(int index) {
    CTTask* task = cron->tasks().at(index);
    auto* newTask = new CTTask(*task);
    newTask->comment = newTask->comment + QChar(0x20) + tr("(Copy)");
    cron->addTask(newTask);
    cron->save();
    view();
}

void TaskDelegate::createEntry() {
    auto* task = new CTTask({}, {}, cron->userLogin(), false);
    auto* td = new TaskDialog(task, tr("New Task"), widget);
    td->show();
    QApplication::connect(td, &TaskDialog::accepted, td, [this, task] {
        cron->addTask(task);
        cron->save();
        view();
    });
}

void TaskDelegate::modifyEntry(int index) {
    CTTask* task = cron->tasks().at(index);
    auto* td = new TaskDialog(task, tr("Edit Task"), widget);
    td->show();
    QApplication::connect(td, &TaskDialog::accepted, td, [this, task] {
        cron->modifyTask(task);
        cron->save();
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
    setIcon(widget->item(index), task->enabled);
}
