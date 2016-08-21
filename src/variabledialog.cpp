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

#include "ctvariable.h"
#include "variabledialog.h"
#include "ui_variabledialog.h"

VariableDialog::VariableDialog(CTVariable* _ctVar, const QString& _caption, QWidget *parent) :
    BaseDialog(parent),
    ui(new Ui::VariableDialog),
    variable(_ctVar)
{
    ui->setupUi(this);
    setWindowTitle(_caption);
    ui->varEdit->setText(variable->variable);
    ui->valEdit->setText(variable->value);
    ui->commentEdit->setText(variable->comment);
    // ButtonBox action
    connect(ui->varEdit, SIGNAL(textChanged(QString)), SLOT(validate()));
    connect(ui->valEdit, SIGNAL(textChanged(QString)), SLOT(validate()));
    if(ui->varEdit->text().isEmpty() || ui->valEdit->text().isEmpty())
        ui->buttonBox->setDisabled(true);
}

VariableDialog::~VariableDialog()
{
    delete ui;
}

void VariableDialog::validate() {
    if(ui->varEdit->text().isEmpty() || ui->valEdit->text().isEmpty()) {
        ui->errorLabel->setText("<b>fill both var and value</b>");
        ui->buttonBox->setDisabled(true);
    } else {
        ui->errorLabel->clear();
        ui->buttonBox->setEnabled(true);
    }
}

void VariableDialog::on_buttonBox_accepted() {
    variable->variable = ui->varEdit->text();
    variable->value = ui->valEdit->text();
    variable->comment = ui->commentEdit->text();
    emit accepted(variable);
    this->close();
}

void VariableDialog::on_buttonBox_rejected() {
    this->close();
}
