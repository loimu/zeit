/* ========================================================================
*    Copyright (C) 2015-2021 Blaze <blaze@vivaldi.net>
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

#ifndef BASEDELEGATE_H
#define BASEDELEGATE_H

#include <QCoreApplication>
#include "config.h"

#ifdef BUILD_HELPER
  #define ROOT_ACTIONS cron->isSystemCron()
#else
  #define ROOT_ACTIONS false
#endif // BUILD_HELPER

class QListWidget;
class QListWidgetItem;

class BaseDelegate
{
public:
    QString caption{};
    QString toolTip{};

    BaseDelegate(QListWidget* widget = nullptr);
    virtual ~BaseDelegate() {}

    void enableElidedText(bool enabled) { isElidedTextEnabled = enabled; }

    virtual void view()=0;
    virtual void copyEntry(int index)=0;
    virtual void createEntry()=0;
    virtual void modifyEntry(int index)=0;
    virtual void deleteEntry(int index)=0;
    virtual void toggleEntry(int index)=0;

protected:
    bool isElidedTextEnabled = false;
    QListWidget* widget;

    void setIcon(QListWidgetItem* item, bool enabled);
    QString elideText(const QString& text) const;
};

#endif // BASEDELEGATE_H
