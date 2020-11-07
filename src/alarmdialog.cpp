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

#include "cttask.h"
#include "alarmdialog.h"
#include "ui_alarmdialog.h"


AlarmDialog::AlarmDialog(CTTask* _ctTask, QWidget* parent) :
    BaseDialog(parent),
    task(_ctTask),
    ui(new Ui::AlarmDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("New Alarm"));
    /* prepopulate fields */
    ui->checkBoxMon->setChecked(true);
    ui->checkBoxTue->setChecked(true);
    ui->checkBoxWed->setChecked(true);
    ui->checkBoxThu->setChecked(true);
    ui->checkBoxFri->setChecked(true);
    ui->lineEditComment->setText(tr("New Alarm"));
    /* detect player */
    QProcess proc;
    proc.start(QStringLiteral("which"),
               QStringList{QStringLiteral("mpv"), QStringLiteral("mplayer")});
    proc.waitForFinished();
    QStringList players = QString::fromUtf8(proc.readAllStandardOutput())
            .split(QRegExp(QStringLiteral("\n")));
    if(players.length() > 0)
        ui->lineEditPlayer->setText(players.at(0));
    /* file dialog actions */
    ui->pushButtonPlayer->setIcon(
                QIcon::fromTheme(QStringLiteral("document-open")));
    ui->pushButtonSoundFile->setIcon(
                QIcon::fromTheme(QStringLiteral("document-open")));
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
    /* dialog actions */
    connect(ui->pushButtonReset, &QPushButton::released, this, [=] {
        ui->spinBoxHour->setValue(0);
        ui->spinBoxMinute->setValue(0);
    });
    connect(ui->pushButtonCurrent, &QPushButton::released,
            this, &AlarmDialog::setCurrentTime);
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &AlarmDialog::save);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &AlarmDialog::close);
    connect(ui->lineEditPlayer, &QLineEdit::textEdited,
            this, [this] { validate(*(ui->lineEditPlayer)); });
    connect(ui->lineEditSoundFile, &QLineEdit::textEdited,
            this, [this] { validate(*(ui->lineEditSoundFile)); });
    setCurrentTime();
}

AlarmDialog::~AlarmDialog() {
    delete ui;
}

void AlarmDialog::setCurrentTime() {
    ui->spinBoxHour->setValue(QTime::currentTime().hour());
    ui->spinBoxMinute->setValue(QTime::currentTime().minute());
}

void AlarmDialog::save() {
    emit ui->lineEditPlayer->textEdited(QString());
    emit ui->lineEditSoundFile->textEdited(QString());
    if(!isInputValid)
        return;
    task->comment = ui->lineEditComment->text();
    task->command = QString(QStringLiteral("%1 \"%2\""))
            .arg(ui->lineEditPlayer->text(),
                 ui->lineEditSoundFile->text());
    task->hour.setEnabled(ui->spinBoxHour->value(), true);
    task->minute.setEnabled(ui->spinBoxMinute->value(), true);
    const QVector<QCheckBox*> checkboxes {
        ui->checkBoxMon, ui->checkBoxTue, ui->checkBoxWed, ui->checkBoxThu,
                ui->checkBoxFri, ui->checkBoxSat, ui->checkBoxSun};
    for(int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++)
        task->dayOfWeek.setEnabled(dw, checkboxes.at(dw)->isChecked());
    for(int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++)
        task->dayOfMonth.setEnabled(dm, true);
    for(int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++)
        task->month.setEnabled(mo, true);
    emit accepted();
    this->close();
}

void AlarmDialog::validate(QLineEdit& input) {
    if(input.text().isEmpty()) {
        isInputValid = false;
        input.setToolTip(tr("This field should not be empty"));
        input.setStyleSheet(
                    QStringLiteral("border:1.5px solid red;border-radius:5px;"));
    } else {
        isInputValid = true;
        input.setToolTip(QString());
        input.setStyleSheet(QString());
    }
}
