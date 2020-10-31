/* ========================================================================
*    Copyright (C) 2015-2020 Blaze <blaze@vivaldi.net>
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
    BaseEditDialog(parent),
    ui(new Ui::CommandDialog),
    commands(commands_)
{
    ui->setupUi(this);
    ui->errorLabel->setVisible(false);
    errorLabel = ui->errorLabel;
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
}

CommandDialog::~CommandDialog() {
    delete ui;
}

void CommandDialog::save() {
    if(ui->lineEditCommand->text().isEmpty()) {
        showError(tr("Command field should not be empty"));
        return;
    }
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
