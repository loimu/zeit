/* ========================================================================
*    Copyright (C) 2015-2022 Blaze <blaze@vivaldi.net>
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

#ifndef TIMERDIALOG_H
#define TIMERDIALOG_H


#include "basedialog.h"


namespace Ui {
class TimerDialog;
}
class Commands;
class QLineEdit;

class TimerDialog : public BaseDialog
{
    Q_OBJECT

public:
    explicit TimerDialog(Commands* commands, QWidget* parent = nullptr);
    ~TimerDialog();
    Q_SIGNAL void accepted();

private:
    bool isInputValid = false;
    Ui::TimerDialog* ui;
    Commands* commands;
    void save();
    void validate(QLineEdit* input);
};

#endif // TIMERDIALOG_H
