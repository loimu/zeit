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

#include "ctvariable.h"
#include "variabledialog.h"
#include "ui_variabledialog.h"


VariableDialog::VariableDialog(CTVariable* _ctVar,
                               const QString& _caption, QWidget *parent) :
    BaseEditDialog(parent),
    ui(new Ui::VariableDialog),
    variable(_ctVar)
{
    ui->setupUi(this);
    ui->errorLabel->setVisible(false);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    errorLabel = ui->errorLabel;
    setWindowTitle(_caption);
    ui->varEdit->setText(variable->variable);
    ui->valEdit->setText(variable->value);
    ui->commentEdit->setText(variable->comment);
}

VariableDialog::~VariableDialog()
{
    delete ui;
}

void VariableDialog::on_buttonBox_accepted() {
    if(ui->varEdit->text().isEmpty()) {
        showError(tr("Variable field should not be empty"));
        return;
    }
    if(ui->varEdit->text().contains(QRegExp(QStringLiteral("\\W")))) {
        showError(tr("Invalid variable name"));
        return;
    }
    variable->variable = ui->varEdit->text();
    variable->value = ui->valEdit->text();
    variable->comment = ui->commentEdit->text();
    emit accepted(variable);
    this->close();
}

void VariableDialog::on_buttonBox_rejected() {
    this->close();
}
