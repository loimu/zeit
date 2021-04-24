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

#include <QProcess>
#include <QFileDialog>
#include <QTime>

#include "commands.h"
#include "commanddialog.h"
#include "ui_commanddialog.h"


CommandDialog::CommandDialog(Commands* commands_, QWidget* parent) :
    BaseDialog(parent),
    ui(new Ui::CommandDialog),
    commands(commands_)
{
    ui->setupUi(this);
    setWindowTitle(tr("New Command"));
    ui->lineEditComment->setText(tr("New Command"));
    ui->checkBox->setChecked(true);
    /* get system time with a positive offset of 5 minutes */
    QTime time = QTime::currentTime().addSecs(60 * 5);
    ui->spinBoxHours->setValue(time.hour());
    ui->spinBoxMinutes->setValue(time.minute());
    /* dialog actions */
    connect(ui->pushButtonCurrent, &QPushButton::released, this, [=] {
        ui->spinBoxHours->setValue(QTime::currentTime().hour());
        ui->spinBoxMinutes->setValue(QTime::currentTime().minute());
    });
    connect(ui->pushButtonReset, &QPushButton::released, this, [=] {
        ui->spinBoxHours->setValue(0);
        ui->spinBoxMinutes->setValue(0);
    });
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &CommandDialog::save);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &CommandDialog::close);
    connect(ui->lineEditCommand, &QLineEdit::textEdited, this, [this] {
        if(ui->lineEditCommand->text().isEmpty()) {
            const QString errorStyleSheet =
                    QStringLiteral("border:1.5px solid red;border-radius:5px;");
            ui->lineEditCommand->setStyleSheet(errorStyleSheet);
            ui->lineEditCommand->setToolTip(
                        tr("Command field should not be empty"));
            isInputValid = false;
        } else {
            ui->lineEditCommand->setStyleSheet(QString());
            ui->lineEditCommand->setToolTip(QString());
            isInputValid = true;
        }
    });
}

CommandDialog::~CommandDialog() {
    delete ui;
}

void CommandDialog::save() {
    emit ui->lineEditCommand->textEdited(QString());
    if(!isInputValid)
        return;
    QString command = ui->lineEditCommand->text();
    if(ui->checkBox->isChecked())
        command.append(QString(QStringLiteral(" & notify-send Command \"%1\""))
                       .arg(ui->lineEditComment->text()));
    QString time = QString(QStringLiteral("%1:%2"))
            .arg(ui->spinBoxHours->value())
            .arg(ui->spinBoxMinutes->value(), 2, 10, QChar::fromLatin1('0'));
    commands->addCommand(command.toLocal8Bit(), time);
    emit accepted();
    this->close();
}
