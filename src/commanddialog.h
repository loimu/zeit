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

#ifndef COMMANDDIALOG_H
#define COMMANDDIALOG_H

#include "baseeditdialog.h"

namespace Ui {
class CommandDialog;
}

class Commands;

class CommandDialog : public BaseEditDialog
{
    Q_OBJECT

    Ui::CommandDialog *ui;
    Commands* commands;

public:
    explicit CommandDialog(Commands* commands, QWidget *parent = nullptr);
    ~CommandDialog();

signals:
    void accepted();

private slots:
    void on_pushButtonCurrent_released();
    void on_pushButtonReset_released();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // COMMANDDIALOG_H
