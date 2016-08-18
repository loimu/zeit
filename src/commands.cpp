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

#include <QProcess>
#include <QRegExp>
#include <QDate>

#include "commands.h"

Commands::Commands() : commands(new QList<Command>)
{
    QProcess proc;
    proc.start(QStringLiteral("atq"));
    proc.waitForFinished(-1);
    QString output = QString::fromUtf8(proc.readAllStandardOutput());
    QStringList entries;
    if(!output.isEmpty())
        entries = output.split("\n", QString::SkipEmptyParts);
    for(QString entry : entries) {
        QRegExp match(QStringLiteral("^(\\d+)\\s+(.*)$"));
        match.setMinimal(true);
        match.indexIn(entry);
        Command command;
        command.id = match.cap(1);
        command.description = match.cap(2);
        command.command = QStringLiteral("n/a");
        commands->append(command);
    }
}

bool Commands::execute(QString program, QStringList options) {
    QProcess proc;
    proc.start(program, options);
    proc.waitForFinished(-1);
    // returns true (success) if no output in standard error
    return proc.readAllStandardError().isEmpty();
}

void Commands::addCommand(QString command, QString time) {
    QString cmdline = QString(QStringLiteral("echo '%1' | at %2"))
            .arg(command)
            .arg(time);
    execute(QStringLiteral("bash"), QStringList{QStringLiteral("-c"), cmdline});
}

void Commands::deleteCommand(int index) {
    if(execute(QStringLiteral("atrm"), QStringList{commands->at(index).id}))
        commands->removeAt(index);
}

QList<Command>* Commands::getCommands() {
    return commands;
}
