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

#include "cttask.h"
#include "ctcron.h"

#include "listitem.h"

ListItem::ListItem(CTTask* _cttask, QListWidget *parent, int type) :
    QListWidgetItem(QString(), parent, type)
{
    ctTask = _cttask;
    QString task;
    task.append(QString("Command: %1\n").arg(ctTask->command));
    task.append(QString("Description: %1\n").arg(ctTask->comment));
    task.append(QString("Periodicity: %1").arg(ctTask->describe()));
    this->setText(task);
}

CTTask* ListItem::task() {
    return ctTask;
}
