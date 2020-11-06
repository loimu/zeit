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
#include "timerdialog.h"
#include "ui_timerdialog.h"


TimerDialog::TimerDialog(Commands* commands_, QWidget* parent) :
    BaseEditDialog(parent),
    ui(new Ui::TimerDialog),
    commands(commands_)
{
    ui->setupUi(this);
    ui->errorLabel->setVisible(false);
    errorLabel = ui->errorLabel;
    setWindowTitle(tr("New Timer"));
    /* prepopulate fields */
    ui->lineEditComment->setText(tr("New Timer"));
    ui->checkBox->setChecked(true);
    /* detect player */
    QProcess proc;
    proc.start(QStringLiteral("which"), QStringList{QStringLiteral("mpv"),
                                                    QStringLiteral("mplayer")});
    proc.waitForFinished(-1);
    QStringList players = QString::fromUtf8(proc.readAllStandardOutput()).split(
                QRegExp(QStringLiteral("\n")));
    if(players.length() > 0)
        ui->lineEditPlayer->setText(players.at(0));
    /* get system time with a positive offset of 5 minutes */
    QTime time = QTime::currentTime().addSecs(60 * 5);
    ui->spinBoxHours->setValue(time.hour());
    ui->spinBoxMinutes->setValue(time.minute());
    /* file dialog actions */
    connect(ui->pushButtonSoundFile, &QPushButton::released, this, [=] {
        QString file = QFileDialog::getOpenFileName(
                    this,
                    QStringLiteral("Sound File"),
                    QDir::homePath(),
                    QStringLiteral("Media (*.wav *.ogg *.mp3 *.flac)"));
        ui->lineEditSoundFile->setText(file);
    });
    connect(ui->pushButtonPlayer, &QPushButton::released, this, [=] {
        QFileDialog* fd = new QFileDialog(this, QStringLiteral("Player"),
                                          QDir::homePath());
        fd->setMimeTypeFilters(
                    QStringList{QStringLiteral("application/x-executable"),
                                QStringLiteral("application/x-sharedlib"),
                                QStringLiteral("application/x-shellscript"),
                                QStringLiteral("text/x-python")});
        if(fd->exec())
            ui->lineEditPlayer->setText(fd->getOpenFileName());
    });
    ui->pushButtonPlayer->setIcon(QIcon::fromTheme(
                                      QStringLiteral("document-open")));
    ui->pushButtonSoundFile->setIcon(QIcon::fromTheme(
                                         QStringLiteral("document-open")));
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
            this, &TimerDialog::save);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &TimerDialog::close);
}

TimerDialog::~TimerDialog() {
    delete ui;
}

void TimerDialog::save() {
    if(ui->lineEditPlayer->text().isEmpty()) {
        showError(tr("Player field should not be empty"));
        return;
    }
    if(ui->lineEditSoundFile->text().isEmpty()) {
        showError(tr("Soundfile field should not be empty"));
        return;
    }
    QString command = QString(QStringLiteral("%1 \"%2\""))
            .arg(ui->lineEditPlayer->text(), ui->lineEditSoundFile->text());
    if(ui->checkBox->isChecked())
        command.append(QString(QStringLiteral(" & notify-send Timer \"%1\""))
                       .arg(ui->lineEditComment->text()));
    QString time = QString(QStringLiteral("%1:%2"))
            .arg(ui->spinBoxHours->value())
            .arg(ui->spinBoxMinutes->value(), 2, 10, QChar::fromLatin1('0'));
    commands->addCommand(command.toLocal8Bit(), time);
    emit accepted();
    this->close();
}
