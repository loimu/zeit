/* ========================================================================
*    Copyright (C) 2015-2019 Blaze <blaze@vivaldi.net>
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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <QVector>
#include <QMap>


struct Command {
    uint id;
    QString command;
    QString description;
};

class Commands
{
    QVector<Command> commands; // vector of commands taken from atq
    QMap<uint, QString> map;   // preserves command contents during session

public:
    void addCommand(const QByteArray &command, const QString &time);
    void deleteCommand(int index);
    const QVector<Command>& getCommands();
};

#endif // COMMANDS_H
