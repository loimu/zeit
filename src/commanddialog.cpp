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

#include <QProcess>
#include <QFileDialog>
#include <QTime>

#include "commands.h"
#include "commanddialog.h"
#include "ui_commanddialog.h"

CommandDialog::CommandDialog(Commands* commands_, QWidget *parent) :
    BaseEditDialog(parent),
    ui(new Ui::CommandDialog),
    commands(commands_)
{
    ui->setupUi(this);
    setWindowTitle(tr("New Command"));
    ui->lineEditComment->setText(tr("New Command"));
    ui->checkBox->setChecked(true);
    // get system time with a positive offset of 5 minutes
    QTime time = QTime::currentTime().addSecs(60 * 5);
    ui->spinBoxHours->setValue(time.hour());
    ui->spinBoxMinutes->setValue(time.minute());
}

CommandDialog::~CommandDialog()
{
    delete ui;
}

void CommandDialog::on_pushButtonCurrent_released() {
    ui->spinBoxHours->setValue(QTime::currentTime().hour());
    ui->spinBoxMinutes->setValue(QTime::currentTime().minute());
}

void CommandDialog::on_pushButtonReset_released() {
    ui->spinBoxHours->setValue(0);
    ui->spinBoxMinutes->setValue(0);
}

void CommandDialog::on_buttonBox_accepted() {
    QString command = QString(QStringLiteral("%1 & "))
            .arg(ui->lineEditCommand->text());
    if(ui->checkBox->isChecked())
        command.append(QString(QStringLiteral("notify-send Timer \\\"%1\\\""))
                       .arg(ui->lineEditComment->text()));
    QString time = QString(QStringLiteral("%1:%2"))
            .arg(ui->spinBoxHours->value())
            .arg(ui->spinBoxMinutes->value(), 2, 10, QChar('0'));
    commands->addCommand(command, time);
    emit accepted();
    this->close();
}

void CommandDialog::on_buttonBox_rejected() {
    this->close();
}
