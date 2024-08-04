/* ========================================================================
*    Copyright (C) 2015-2024 Blaze <blaze@vivaldi.net>
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

#include <QRegularExpression>

#include "cttask.h"
#include "taskdialog.h"
#include "ui_taskdialog.h"

TaskDialog::TaskDialog(CTTask* _ctTask,
                       const QString& _caption, QWidget *parent) :
    BaseDialog(parent),
    task(_ctTask),
    ui(new Ui::TaskDialog)
{
    ui->setupUi(this);
    setWindowTitle(_caption);
    ui->radioBasic->setChecked(true);
    toggleMode();
    QStringList entries;
    entries << tr("Every minute") << tr("Every hour") << tr("Every day")
            << tr("Every week") << tr("Every month") <<  tr("Every weekday");
    ui->comboBox->addItems(entries);
    helpToolTip = tr(
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
    ui->commandEdit->setText(task->command);  // fill form fields
    ui->commentEdit->setText(task->comment);
    ui->enabledCheckBox->setChecked(task->enabled);
    messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);
    ui->scrollArea->setWidget(messageLabel);
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
    const QVector<QLineEdit*>& leVector { ui->commandEdit, ui->editMinute,
                ui->editHour, ui->editDay, ui->editWeekday, ui->editMonth };
    for(QLineEdit* le : leVector) {
        connect(le, &QLineEdit::textEdited, this, &TaskDialog::updateDialog);
    }
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
        static const QRegularExpression whitespaceRx(QStringLiteral("\\s+"));
        QStringList tokenList = task->schedulingCronFormat()
                                    .split(whitespaceRx);
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
        setText("0", "*", "*", "*", "*");  // every hour
        break;
    case 2:
        setText("0", "0", "*", "*", "*");  // every day
        break;
    case 3:
        setText("0", "0", "*", "1", "*");  // every week
        break;
    case 4:
        setText("0", "0", "1", "*", "*");  // every month
        break;
    case 5:
        setText("1", "8", "*", "1-5", "*");  // every weekday at 8am
        break;
    default:
        setText("*", "*", "*", "*", "*");  // every minute
        break;
    }
    updateDialog();
}

void TaskDialog::save() {
    validate();
    if(!isInputValid)
        return;
    task->command = ui->commandEdit->text();
    task->comment = ui->commentEdit->text();
    task->enabled = ui->enabledCheckBox->isChecked();
    emit accepted();
    this->close();
}

void TaskDialog::updateDialog() {
    validate();
    if(isInputValid) {
        /* write time tokens into cttask object */
        setUnit(task->minute, ui->editMinute->text());
        setUnit(task->hour, ui->editHour->text());
        setUnit(task->dayOfMonth, ui->editDay->text());
        setUnit(task->dayOfWeek, ui->editWeekday->text());
        setUnit(task->month, ui->editMonth->text());
        messageLabel->setText(task->describe());
    }
    else
        messageLabel->clear();
}

void TaskDialog::validate() {
    isInputValid = true;
    ui->commandEdit->setStyleSheet(QString());
    ui->commandEdit->setToolTip(QString());
    const QString errorStyleSheet =
            QStringLiteral("border:1.5px solid red;border-radius:5px;");
    if(ui->commandEdit->text().isEmpty()) {
        ui->commandEdit->setStyleSheet(errorStyleSheet);
        ui->commandEdit->setToolTip(tr("Command field should not be empty"));
        isInputValid = false;
    }
    const QVector<QLineEdit*> leVector {
        ui->editMinute,ui->editHour,ui->editDay,ui->editWeekday,ui->editMonth };
    for(QLineEdit* le : leVector) {
        le->setStyleSheet(QString());
        le->setToolTip(helpToolTip);
        static const QRegularExpression rx(
            QRegularExpression::anchoredPattern(
                QStringLiteral("\\*|\\d{1,2}(,\\d{1,2}|-\\d{1,2}(/\\d{1,2})?)*")));
        const QRegularExpressionMatch match = rx.match(le->text());
        if(!match.hasMatch()) {
            le->setStyleSheet(errorStyleSheet);
            le->setToolTip(tr("<b>Invalid input</b><br />") + helpToolTip);
            isInputValid = false;
        }
    }
}
