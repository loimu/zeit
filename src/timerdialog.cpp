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

#include <QProcess>
#include <QFileDialog>
#include <QTime>

#include "timerdialog.h"
#include "ui_timerdialog.h"

TimerDialog::TimerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimerDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("New Timer"));
    // prepopulate fields
    ui->lineEditComment->setText(tr("New Timer"));
    ui->checkBox->setChecked(true);
    // detect player
    QProcess proc;
    proc.start("which", QStringList() << "mpv" << "mplayer");
    proc.waitForFinished(-1);
    QStringList players = QString::fromUtf8(proc.readAllStandardOutput()).split(QRegExp("\n"));
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
    /*
        timer uses the "at" cli utility: echo '/path/to/script' | at 11:15
    */
    QString notification = QString();
    if(ui->checkBox->isChecked())
        notification.append(QString(" ; notify-send \"Timer\" \"%1\"")
                       .arg(ui->lineEditComment->text()));
    QString command = QString("echo '%1 %2%3' | at %4:%5")
            .arg(ui->lineEditPlayer->text())
            .arg(ui->lineEditSoundFile->text())
            .arg(notification)
            .arg(ui->spinBoxHours->value())
            .arg(ui->spinBoxMinutes->value());
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << command);
    proc.waitForFinished(-1);
    QString output = QString::fromUtf8(proc.readAllStandardOutput());
}

void TimerDialog::showFileDialog() {
    QStringList file = QFileDialog::getOpenFileNames(this, "Sound file",
                                                     QDir::homePath(),
                                            "Media (*.wav *.ogg *.mp3 *.flac)");
    if(file.length() > 0)
        ui->lineEditSoundFile->setText(file.at(0));
}

void TimerDialog::showPlayerDialog() {
    QStringList file = QFileDialog::getOpenFileNames(this,"Executable","/usr/bin","");
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
