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

#include <QApplication>
#include <QListWidget>

#include "ctcron.h"
#include "ctvariable.h"
#include "variabledialog.h"
#include "variabledelegate.h"


VariableDelegate::VariableDelegate(QListWidget* widget, CTCron* cron_)
    : BaseDelegate(widget),
      cron(cron_)
{
    caption = tr("Variable");
    toolTip = tr("environment variables for crontab");
}

void VariableDelegate::view() {
    widget->setEnabled(cron->isCurrentUserCron() || ROOT_ACTIONS);
    widget->clear();
    for(CTVariable* var: cron->variables()) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString(QStringLiteral("%1%2=%3"))
                      .arg(var->comment.isEmpty()
                           ? QString()
                           : QString(QStringLiteral("## %1\n")).arg(var->comment),
                           var->variable, var->value));
        setIcon(item, var->enabled);
        widget->addItem(item);
    }
}

void VariableDelegate::copyEntry(int index) {
    CTVariable* var = cron->variables().at(index);
    auto* newVariable = new CTVariable(*var);
    newVariable->comment = newVariable->comment + QChar(0x20) + tr("(Copy)");
    cron->addVariable(newVariable);
    cron->save();
    view();
}

void VariableDelegate::createEntry() {
    auto* var = new CTVariable({}, {}, cron->userLogin());
    auto* vd = new VariableDialog(var, tr("New Variable"), widget);
    vd->show();
    QApplication::connect(vd, &VariableDialog::accepted, vd, [this, var] {
        cron->addVariable(var);
        cron->save();
        view();
    });
}

void VariableDelegate::modifyEntry(int index) {
    CTVariable* var = cron->variables().at(index);
    auto* vd = new VariableDialog(var, tr("Edit Variable"), widget);
    vd->show();
    QApplication::connect(vd, &VariableDialog::accepted, vd, [this, var] {
        cron->modifyVariable(var);
        cron->save();
        view();
    });
}

void VariableDelegate::deleteEntry(int index) {
    CTVariable* var = cron->variables().at(index);
    cron->removeVariable(var);
    cron->save();
}

void VariableDelegate::toggleEntry(int index) {
    CTVariable* var = cron->variables().at(index);
    var->enabled = !var->enabled;
    cron->save();
    setIcon(widget->item(index), var->enabled);
}
