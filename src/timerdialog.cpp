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

#include "timerdialog.h"
#include "ui_timerdialog.h"

TimerDialog::TimerDialog(const QString& _caption, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimerDialog)
{
    ui->setupUi(this);
    setWindowTitle(_caption);
    // detect player
    QProcess proc;
    proc.start("which", QStringList() << "mpv");
    proc.waitForFinished(-1);
    ui->lineEditPlayer->setText(QString::fromUtf8(proc.readAllStandardOutput())
                                                  .remove(QRegExp("[\n\t\r]")));
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
    QString command = QString("echo '%1 %2' | at %3:%4")
            .arg(ui->lineEditPlayer->text())
            .arg(ui->lineEditSoundFile->text())
            .arg(ui->spinBoxHours->value())
            .arg(ui->spinBoxMinutes->value());
    if(ui->checkBox->isChecked())
        command.append(QString(" ; notify-send 'Timer' '%1'")
                       .arg(ui->lineEditComment->text()));
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << command);
    proc.waitForFinished(-1);
    QString output = QString::fromUtf8(proc.readAllStandardOutput());
}

void TimerDialog::showFileDialog() {
    QStringList file = QFileDialog::getOpenFileNames(this, "Sound file", "",
                                            "Media (*.wav *.ogg *.mp3 *.flac)");
    if(file.length() > 0)
        ui->lineEditSoundFile->setText(file.at(0));
}

void TimerDialog::showPlayerDialog() {
    QStringList file = QFileDialog::getOpenFileNames(this,"Executable","/usr/bin","");
    if(file.length() > 0)
        ui->lineEditPlayer->setText(file.at(0));
}
