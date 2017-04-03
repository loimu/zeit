/* ========================================================================
*    Copyright (C) 2015-2017 Blaze <blaze@vivaldi.net>
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


TimerDialog::TimerDialog(Commands* commands_, QWidget *parent) :
    BaseEditDialog(parent),
    ui(new Ui::TimerDialog),
    commands(commands_)
{
    ui->setupUi(this);
    ui->errorLabel->setVisible(false);
    errorLabel = ui->errorLabel;
    setWindowTitle(tr("New Timer"));
    // prepopulate fields
    ui->lineEditComment->setText(tr("New Timer"));
    ui->checkBox->setChecked(true);
    // detect player
    QProcess proc;
    proc.start(QStringLiteral("which"), QStringList{QLatin1String("mpv"),
                                                    QLatin1String("mplayer")});
    proc.waitForFinished(-1);
    QStringList players = QString::fromUtf8(
                proc.readAllStandardOutput())
            .split(QRegExp(QStringLiteral("\n")));
    if(players.length() > 0)
        ui->lineEditPlayer->setText(players.at(0));
    // get system time with a positive offset of 5 minutes
    QTime time = QTime::currentTime().addSecs(60 * 5);
    ui->spinBoxHours->setValue(time.hour());
    ui->spinBoxMinutes->setValue(time.minute());
    // FileDialog actions
    connect(ui->pushButtonSoundFile, SIGNAL(released()),
            SLOT(showFileDialog()));
    connect(ui->pushButtonPlayer, SIGNAL(released()), SLOT(showPlayerDialog()));
    ui->pushButtonPlayer->setIcon(QIcon::fromTheme(
                                      QStringLiteral("document-open")));
    ui->pushButtonSoundFile->setIcon(QIcon::fromTheme(
                                         QStringLiteral("document-open")));
}

TimerDialog::~TimerDialog()
{
    delete ui;
}

void TimerDialog::showFileDialog() {
    QString file = QFileDialog::getOpenFileName(
                this,
                QStringLiteral("Sound File"),
                QDir::homePath(),
                QStringLiteral("Media (*.wav *.ogg *.mp3 *.flac)"));
    ui->lineEditSoundFile->setText(file);
}

void TimerDialog::showPlayerDialog() {
    QFileDialog* fd = new QFileDialog(this, QStringLiteral("Player"),
                                      QDir::homePath());
    fd->setMimeTypeFilters(
                QStringList{QLatin1String("application/x-executable"),
                            QLatin1String("application/x-sharedlib"),
                            QLatin1String("application/x-shellscript"),
                            QLatin1String("text/x-python")});
    if(fd->exec())
        ui->lineEditPlayer->setText(fd->getOpenFileName());
}

void TimerDialog::on_pushButtonCurrent_released() {
    QTime time = QTime::currentTime();
    ui->spinBoxHours->setValue(time.hour());
    ui->spinBoxMinutes->setValue(time.minute());
}

void TimerDialog::on_pushButtonReset_released() {
    ui->spinBoxHours->setValue(0);
    ui->spinBoxMinutes->setValue(0);
}

void TimerDialog::on_buttonBox_accepted() {
    if(ui->lineEditPlayer->text().isEmpty()) {
        showError(tr("Player field should not be empty"));
        return;
    }
    if(ui->lineEditSoundFile->text().isEmpty()) {
        showError(tr("Soundfile field should not be empty"));
        return;
    }
    QString command = QString(QLatin1String("%1 \\\"%2\\\" & "))
            .arg(ui->lineEditPlayer->text())
            .arg(ui->lineEditSoundFile->text());
    if(ui->checkBox->isChecked())
        command.append(QString(QLatin1String("notify-send Timer \\\"%1\\\""))
                       .arg(ui->lineEditComment->text()));
    QString time = QString(QLatin1String("%1:%2"))
            .arg(ui->spinBoxHours->value())
            .arg(ui->spinBoxMinutes->value(), 2, 10, QChar('0'));
    commands->addCommand(command, time);
    emit accepted();
    this->close();
}

void TimerDialog::on_buttonBox_rejected() {
    this->close();
}
