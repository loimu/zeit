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

#include <QApplication>
#include <QListWidget>

#include "commands.h"
#include "commanddialog.h"
#include "commanddelegate.h"


CommandDelegate::CommandDelegate(QListWidget* widget, Commands* commands_)
    : BaseDelegate(widget),
      commands(commands_)
{
    caption = tr("Command");
    toolTip = tr("commands, scheduled to be executed once");
}

void CommandDelegate::view() {
    widget->setEnabled(true);
    widget->clear();
    for(const Command& c: commands->getCommands()) {
        QListWidgetItem* item = new QListWidgetItem(
                    elideText(c.description) + QChar::fromLatin1('\n') +
                    elideText(tr("Command: ") + c.command));
        widget->addItem(item);
    }
}

void CommandDelegate::createEntry() {
    CommandDialog* cd = new CommandDialog(commands, widget);
    cd->show();
    QApplication::connect(cd, &CommandDialog::accepted, cd, [this] { view(); });
}

void CommandDelegate::deleteEntry(int index) {
    commands->deleteCommand(index);
}
