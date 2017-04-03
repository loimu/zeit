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

#ifndef ALARMDIALOG_H
#define ALARMDIALOG_H

#include "baseeditdialog.h"

namespace Ui {
class AlarmDialog;
}

class CTTask;

class AlarmDialog : public BaseEditDialog
{
    Q_OBJECT

    CTTask* task;
    Ui::AlarmDialog *ui;

public:
    explicit AlarmDialog(CTTask*, QWidget *parent = nullptr);
    ~AlarmDialog();

private slots:
    void on_pushButtonCurrent_released();
    void on_pushButtonReset_released();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

signals:
    void accepted(CTTask* task);
};

#endif // ALARMDIALOG_H
