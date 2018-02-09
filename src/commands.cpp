/* ========================================================================
*    Copyright (C) 2015-2018 Blaze <blaze@vivaldi.net>
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
#include <QRegExp>

#include "commands.h"


Commands::Commands() : commands(new QList<Command>),
    map(new QMap<QString, QString>)
{

}

Commands::~Commands() {
    delete map;
    delete commands;
}

void Commands::refresh() {
    commands->clear();
    QProcess p;
    p.start(QStringLiteral("atq"));
    p.waitForFinished();
    QString output = QString::fromUtf8(p.readAllStandardOutput());
    QStringList entries;
    if(!output.isEmpty())
        entries = output.split("\n", QString::SkipEmptyParts);
    for(QString& entry : entries) {
        QRegExp match(QStringLiteral("^(\\d+)\\s+(.*)$"));
        match.setMinimal(true);
        match.indexIn(entry);
        Command command;
        command.id = match.cap(1);
        command.description = match.cap(2);
        command.command = map->value(command.id, QStringLiteral("n/a"));
        commands->append(command);
    }
}

void Commands::addCommand(const QByteArray& command, const QString& time) {
    QProcess p;
    p.start(QStringLiteral("at"), QStringList{time});
    p.write(command);
    p.closeWriteChannel();
    p.waitForFinished();
    QString output = QString::fromUtf8(p.readAllStandardError());
    QRegExp match(QStringLiteral("job\\s(\\d+)\\s(.*)"));
    match.setMinimal(true);
    match.indexIn(output);
    QString id = match.cap(1);
    if(!id.isEmpty())
        map->insert(id, command);
    refresh();
}

void Commands::deleteCommand(int index) {
    QProcess p;
    p.start(QStringLiteral("atrm"), QStringList{commands->at(index).id});
    p.waitForFinished();
    if(p.readAllStandardError().isEmpty())
        commands->removeAt(index);
}

QList<Command>* Commands::getCommands() {
    return commands;
}
