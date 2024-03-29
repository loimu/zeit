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

#ifndef TASKDELEGATE_H
#define TASKDELEGATE_H

#include "basedelegate.h"

class CTCron;

class TaskDelegate : public BaseDelegate
{
    Q_DECLARE_TR_FUNCTIONS(TaskController)

    CTCron* cron;

public:
    TaskDelegate(QListWidget* widget, CTCron* cron);
    void view() override;
    void copyEntry(int index) override;
    void createEntry() override;
    void modifyEntry(int index) override;
    void deleteEntry(int index) override;
    void toggleEntry(int index) override;
};

#endif // TASKDELEGATE_H
