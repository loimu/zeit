/***************************************************************************
 *   CT Month Implementation                                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctHelper.h"

#include <QStringList>

QString CTHelper::exportComment(const QString& comment) {
    if(comment.isEmpty())
        return QString();
    QString result;
    QStringList lines = comment.split(QChar::fromLatin1('\n'));
    for(const QString &line: lines)
        result += QChar::fromLatin1('#') + line + QChar::fromLatin1('\n');
    return result;
}
