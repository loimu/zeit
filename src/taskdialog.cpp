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

#include "cttask.h"
#include "taskdialog.h"
#include "ui_taskdialog.h"

TaskDialog::TaskDialog(CTTask* _ctTask,
                       const QString& _caption, QWidget *parent) :
    BaseEditDialog(parent),
    task(_ctTask),
    ui(new Ui::TaskDialog)
{
    ui->setupUi(this);
    ui->errorLabel->setVisible(false);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    errorLabel = ui->errorLabel;
    setWindowTitle(_caption);
    ui->radioBasic->setChecked(true);
    toggleMode();
    QStringList entries;
    entries << tr("Every minute") << tr("Every hour") << tr("Every day")
                                  << tr("Every week") << tr("Every month");
    ui->comboBox->addItems(entries);
    QString helpToolTip = tr(
                "Valid input examples:"
                "<br/><b>*</b> – all elements are enabled"
                "<br/><b>3</b> – one element is enabled"
                "<br/><b>1,2,5</b> – sequence of three elements"
                "<br/><b>2-7</b> – range from 2 to 7"
                "<br/><b>3-15/2</b> – range from 3 to 15 with step 2"
                "<br/><b>3-15,18,21</b> – combination of the above");
    ui->editMinute->setToolTip(helpToolTip);
    ui->editHour->setToolTip(helpToolTip);
    ui->editDay->setToolTip(helpToolTip);
    ui->editWeekday->setToolTip(helpToolTip);
    ui->editMonth->setToolTip(helpToolTip);
    ui->commandEdit->setText(task->command); // fill form fields
    ui->commentEdit->setText(task->comment);
    init();
    /* switch modes */
    connect(ui->radioAdvanced, &QRadioButton::clicked,
            this, &TaskDialog::toggleMode);
    connect(ui->radioBasic, &QRadioButton::clicked,
            this, &TaskDialog::toggleMode);
    /* other actions */
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::activated),
            this, &TaskDialog::switchPreset);
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &TaskDialog::save);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &TaskDialog::close);
}

TaskDialog::~TaskDialog() {
    delete ui;
}

void TaskDialog::init() {
    if(task->command.isEmpty()) {
        ui->commentEdit->setText(tr("New Task"));
        setText("*", "*", "*", "*", "*");
    }
    else {
        QStringList tokenList = task->schedulingCronFormat()
                .split(QRegExp(QStringLiteral("\\s")));
        setText(tokenList.at(0), tokenList.at(1),
                tokenList.at(2), tokenList.at(4), tokenList.at(3));
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
    if(token != QStringLiteral("*")) {
        for(int i = unit.minimum(); i <= unit.maximum(); i++)
            unit.setEnabled(i, false);
        for(QString& string : token.split(QChar::fromLatin1(','))) {
            /* range detection and interpretation procedure */
            QStringList subStrings = string.split(QChar::fromLatin1('-'));
            int beg = subStrings.at(0).toInt();
            if(subStrings.size() > 1) {
                QStringList subSubStrings = subStrings.at(1)
                        .split(QChar::fromLatin1('/'));
                int end = subSubStrings.at(0).toInt();
                int step = subSubStrings.size() > 1
                        ? subSubStrings.at(1).toInt() : 1;
                for(int i = beg; i <= end; i+=step)
                    unit.setEnabled(i, true);
            } else
            unit.setEnabled(beg, true); // in case if it is not a range
        }
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

void TaskDialog::switchPreset(int index) {
    switch(index) {
    case 1:
        setText("0", "*", "*", "*", "*"); // every hour
        break;
    case 2:
        setText("0", "0", "*", "*", "*"); // every day
        break;
    case 3:
        setText("0", "0", "*", "1", "*"); // every week
        break;
    case 4:
        setText("0", "0", "1", "*", "*"); // every month
        break;
    default:
        setText("*", "*", "*", "*", "*"); // every minute
        break;
    }
}

void TaskDialog::save() {
    if(ui->commandEdit->text().isEmpty()) {
        showError(tr("Command field should not be empty"));
        return;
    }
    QRegExp rx(QStringLiteral("\\*|\\d+(,\\d+|-\\d+(/\\d+)?)*"));
    for(QLineEdit* le : QVector<QLineEdit*> { ui->editMinute, ui->editHour,
        ui->editDay, ui->editWeekday, ui->editMonth }) {
        if(!rx.exactMatch(le->text())) {
            showError(tr("Invalid input in ") + le->objectName());
            return;
        }
    }
    task->command = ui->commandEdit->text();
    task->comment = ui->commentEdit->text();
    /* write time tokens into cttask object */
    setUnit(task->minute, ui->editMinute->text());
    setUnit(task->hour, ui->editHour->text());
    setUnit(task->dayOfMonth, ui->editDay->text());
    setUnit(task->dayOfWeek, ui->editWeekday->text());
    setUnit(task->month, ui->editMonth->text());
    emit accepted(task);
    this->close();
}
