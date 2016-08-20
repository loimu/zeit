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
#include "timerdialog.h"
#include "ui_timerdialog.h"

TimerDialog::TimerDialog(Commands* commands_, QWidget *parent) : QDialog(parent),
    ui(new Ui::TimerDialog),
    commands(commands_)
{
    ui->setupUi(this);
    setWindowTitle(tr("New Timer"));
    // prepopulate fields
    ui->lineEditComment->setText(tr("New Timer"));
    ui->checkBox->setChecked(true);
    // detect player
    QProcess proc;
    proc.start(QStringLiteral("which"), QStringList{QStringLiteral("mpv"),
                                                    QStringLiteral("mplayer")});
    proc.waitForFinished(-1);
    QStringList players = QString::fromUtf8(
                proc.readAllStandardOutput()).split(QRegExp(QStringLiteral("\n")));
    if(players.length() > 0)
        ui->lineEditPlayer->setText(players.at(0));
    // get system time
    ui->spinBoxHours->setValue(QTime::currentTime().hour());
    ui->spinBoxMinutes->setValue(QTime::currentTime().minute() + 5);
    // save task action
    connect(this, SIGNAL(accepted()), SLOT(saveTask()));
    // FileDialog actions
    connect(ui->pushButtonSoundFile, SIGNAL(released()), SLOT(showFileDialog()));
    connect(ui->pushButtonPlayer, SIGNAL(released()), SLOT(showPlayerDialog()));
}

TimerDialog::~TimerDialog()
{
    delete ui;
}

void TimerDialog::saveTask() {
    QString command = QString(QStringLiteral("%1 \\\"%2\\\" & "))
            .arg(ui->lineEditPlayer->text())
            .arg(ui->lineEditSoundFile->text());
    if(ui->checkBox->isChecked())
        command.append(QString(QStringLiteral("notify-send Timer \\\"%1\\\""))
                       .arg(ui->lineEditComment->text()));
    QString time = QString(QStringLiteral("%1:%2"))
            .arg(ui->spinBoxHours->value())
            .arg(ui->spinBoxMinutes->value(), 2, 10, QChar('0'));
    commands->addCommand(command, time);
}

void TimerDialog::showFileDialog() {
    QStringList file = QFileDialog::getOpenFileNames(
                this,
                QStringLiteral("Sound file"),
                QDir::homePath(),
                QStringLiteral("Media (*.wav *.ogg *.mp3 *.flac)"));
    if(file.length() > 0)
        ui->lineEditSoundFile->setText(file.at(0));
}

void TimerDialog::showPlayerDialog() {
    QStringList file = QFileDialog::getOpenFileNames(
                this,
                QStringLiteral("Executable"),
                QDir::homePath(),
                QString());
    if(file.length() > 0)
        ui->lineEditPlayer->setText(file.at(0));
}

void TimerDialog::on_pushButtonCurrent_released() {
    ui->spinBoxHours->setValue(QTime::currentTime().hour());
    ui->spinBoxMinutes->setValue(QTime::currentTime().minute());
}

void TimerDialog::on_pushButtonReset_released() {
    ui->spinBoxHours->setValue(0);
    ui->spinBoxMinutes->setValue(0);
}
