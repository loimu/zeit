/* ========================================================================
*    Copyright (C) 2015-2024 Blaze <blaze@vivaldi.net>
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

#include <QFile>

#include "zeithelper.h"


ActionReply ZeitHelper::save(const QVariantMap& args) {
    qDebug("running actions");
    QString source = args[QLatin1String("source")].toString();
    QString destination = args[QLatin1String("target")].toString();
    if(!QFile::remove(destination))
        qDebug("can't remove file, it doesn't exist");
    if(!QFile::copy(source, destination))
        qDebug("can't write into the system file, something went wrong");
    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("local.zeit.crontab", ZeitHelper)
