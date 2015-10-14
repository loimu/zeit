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

#include "cttask.h"
#include "taskdialog.h"
#include "ui_taskdialog.h"

TaskDialog::TaskDialog(CTTask* _ctTask, const QString& _caption, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskDialog)
{
    task = _ctTask;
    ui->setupUi(this);
    setWindowTitle(_caption);
    ui->radioBasic->setChecked(true);
    toggleMode();
    QStringList entries;
    entries << tr("Every minute") << tr("Every hour") << tr("Every day")
                                  << tr("Every week") << tr("Every month");
    ui->comboBox->addItems(entries);
    // fill form fields
    ui->commandEdit->setText(task->command);
    ui->commentEdit->setText(task->comment);
    init();
    // switch modes
    connect(ui->radioAdvanced, SIGNAL(clicked(bool)), SLOT(toggleMode()));
    connect(ui->radioBasic, SIGNAL(clicked(bool)), SLOT(toggleMode()));
    // ComboBox action
    connect(ui->comboBox, SIGNAL(activated(int)), SLOT(refresh(int)));
    // ButtonBox action
    connect(this, SIGNAL(accepted()), SLOT(saveTask()));
}

TaskDialog::~TaskDialog()
{
    delete ui;
}

void TaskDialog::init() {
    if(task->command.isEmpty()) {
        ui->commentEdit->setText(tr("Undefined"));
        setText("*","*","*","*","*");
    }
    else {
        QStringList tokenList = task->schedulingCronFormat().split(QRegExp("\\s"));
        setText(tokenList.at(0),tokenList.at(1),tokenList.at(2),tokenList.at(4),tokenList.at(3));
    }
}

void TaskDialog::setText(const QString& minute, const QString& hour,
                         const QString& day, const QString& weekday,
                         const QString& month) {
    ui->editMinute->setText(minute);
    ui->editHour->setText(hour);
    ui->editDay->setText(day);
    ui->editWeekday->setText(weekday);
    ui->editMonth->setText(month);
}

void TaskDialog::setUnit(CTUnit& unit, const QString& token) {
    if(token != "*") {
        for(int i = unit.minimum(); i <= unit.maximum(); i++)
            unit.setEnabled(i, false);
        QStringList items = token.split(",");
        foreach(QString string, items)
            unit.setEnabled(string.toInt(), true);
    } else
        for(int i = unit.minimum(); i <= unit.maximum(); i++)
            unit.setEnabled(i, true);
}

void TaskDialog::toggleMode() {
    bool isAdvanced = false;
    if(ui->radioAdvanced->isChecked())
        isAdvanced = true;
    ui->comboBox->setDisabled(isAdvanced);
    ui->editDay->setEnabled(isAdvanced);
    ui->editHour->setEnabled(isAdvanced);
    ui->editMinute->setEnabled(isAdvanced);
    ui->editMonth->setEnabled(isAdvanced);
    ui->editWeekday->setEnabled(isAdvanced);
}

void TaskDialog::saveTask() {
    task->command = ui->commandEdit->text();
    task->comment = ui->commentEdit->text();
    // write time tokens into cttask object
    setUnit(task->minute, ui->editMinute->text());
    setUnit(task->hour, ui->editHour->text());
    setUnit(task->dayOfMonth, ui->editDay->text());
    setUnit(task->dayOfWeek, ui->editWeekday->text());
    setUnit(task->month, ui->editMonth->text());
}

void TaskDialog::refresh(int index) {
    switch(index) {
    case 1:
        setText("0","*","*","*","*"); // every hour
        break;
    case 2:
        setText("0","0","*","*","*"); // every day
        break;
    case 3:
        setText("0","0","*","1","*"); // every week
        break;
    case 4:
        setText("0","0","1","*","*"); // every month
        break;
    default:
        setText("*","*","*","*","*"); // every minute
        break;
    }
}