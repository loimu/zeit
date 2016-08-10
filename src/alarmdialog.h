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

#ifndef ALARMDIALOG_H
#define ALARMDIALOG_H

#include <QDialog>

namespace Ui {
class AlarmDialog;
}

class CTTask;

class AlarmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmDialog(CTTask*, QWidget *parent = nullptr);
    ~AlarmDialog();

private:
    CTTask* task;
    Ui::AlarmDialog *ui;

private slots:
    void saveTask();
    void showFileDialog();
    void showPlayerDialog();
};

#endif // ALARMDIALOG_H
