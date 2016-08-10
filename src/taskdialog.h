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

#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>

namespace Ui {
class TaskDialog;
}

class CTTask;
class CTUnit;

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(CTTask*, const QString&, QWidget *parent = nullptr);
    ~TaskDialog();

private:
    CTTask* task;
    Ui::TaskDialog *ui;
    void init();
    void setText(const QString&, const QString&, const QString&, const QString&, const QString&);
    void setUnit(CTUnit&, const QString&);
private slots:
    void toggleMode();
    void saveTask();
    void refresh(int);
};

#endif // TASKDIALOG_H
