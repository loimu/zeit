/* ========================================================================
*    Copyright (C) 2015-2022 Blaze <blaze@vivaldi.net>
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
                               const QString& _caption, QWidget* parent) :
    BaseDialog(parent),
    ui(new Ui::VariableDialog),
    variable(_ctVar)
{
    ui->setupUi(this);
    setWindowTitle(_caption);
    ui->varEdit->setText(variable->variable);
    ui->valEdit->setText(variable->value);
    ui->commentEdit->setText(variable->comment);
    ui->enabledCheckBox->setChecked(variable->enabled);
    /* dialog actions */
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &VariableDialog::save);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &VariableDialog::close);
    connect(ui->varEdit, &QLineEdit::textChanged, this, [this] {
        isInputValid = true;
        if(ui->varEdit->text().isEmpty()) {
            ui->varEdit->setToolTip(tr("Variable field should not be empty"));
            isInputValid = false;
        }
        if(ui->varEdit->text().contains(QRegExp(QStringLiteral("\\W")))) {
            ui->varEdit->setToolTip(tr("Invalid variable name"));
            isInputValid = false;
        }
        if(isInputValid) {
            ui->varEdit->setStyleSheet(QString());
            ui->varEdit->setToolTip(QString());
        } else {
            const QString errorStyleSheet =
                    QStringLiteral("border:1.5px solid red;border-radius:5px;");
            ui->varEdit->setStyleSheet(errorStyleSheet);
        }
    });
}

VariableDialog::~VariableDialog() {
    delete ui;
}

void VariableDialog::save() {
    emit ui->varEdit->textChanged(QString());
    if(!isInputValid)
        return;
    variable->variable = ui->varEdit->text();
    variable->value = ui->valEdit->text();
    variable->comment = ui->commentEdit->text();
    variable->enabled = ui->enabledCheckBox->isChecked();
    emit accepted();
    this->close();
}
