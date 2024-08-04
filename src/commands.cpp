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

#include <QProcess>
#include <QRegularExpression>

#include "commands.h"


void Commands::addCommand(const QByteArray& command, const QString& time) {
    QProcess p;
    p.start(QStringLiteral("at"), QStringList{time});
    p.write(command);
    p.closeWriteChannel();
    p.waitForFinished();
    QString output = QString::fromUtf8(p.readAllStandardError());
    static const QRegularExpression jobRx(QStringLiteral("job\\s(\\d+?)\\s(.*?)"));
    const QRegularExpressionMatch match = jobRx.match(output);
    if(match.hasMatch()) {
        const uint id = match.captured(1).toUInt();
        map.insert(id, command);
    }
}

void Commands::deleteCommand(int index) {
    QProcess p;
    p.start(QStringLiteral("atrm"),
            QStringList{ QString::number(commands.at(index).id) });
    p.waitForFinished();
    if(p.readAllStandardError().isEmpty())
        commands.removeAt(index);
}

const QVector<Command>& Commands::getCommands() {
    commands.clear();
    QProcess p;
    p.start(QStringLiteral("atq"), QStringList{});
    p.waitForFinished();
    QString output = QString::fromUtf8(p.readAllStandardOutput());
    QStringList entries;
    if(!output.isEmpty())
        entries = output.split(QChar::fromLatin1('\n'), Qt::SkipEmptyParts);
    for(QString& entry : entries) {
        QRegularExpression entryRx(QStringLiteral("^(\\d+?)\\s+?(.*?)$"),
                                   QRegularExpression::MultilineOption);
        QRegularExpressionMatch match = entryRx.match(entry);
        Command command;
        command.id = match.captured(1).toUInt();
        command.description = match.captured(2);
        command.command = map.value(command.id, QStringLiteral("n/a"));
        commands.append(command);
    }
    return commands;
}
