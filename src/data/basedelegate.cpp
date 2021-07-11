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

#include <QListWidget>

#include "basedelegate.h"

#define QSL QStringLiteral


BaseDelegate::BaseDelegate(QListWidget* widget_) : widget(widget_)
{
}

void BaseDelegate::setIcon(QListWidgetItem* item, bool enabled) {
    QString icon = enabled ? QSL("dialog-ok-apply") : QSL("edit-delete");
    item->setIcon(QIcon::fromTheme(icon, QIcon(QSL(":/icons/") + icon)));
}

QString BaseDelegate::elideText(const QString& text) const {
    const QFontMetrics& fm = widget->fontMetrics();
    return fm.elidedText(text,
                         isElidedTextEnabled ? Qt::ElideRight : Qt::ElideNone,
                         widget->width() - 48 /* hardcoded icon size */);
}
