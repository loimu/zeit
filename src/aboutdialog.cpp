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

#include <QKeyEvent>

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QWidget(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_DeleteOnClose);
    addText();
}

AboutDialog::~AboutDialog()
{
    emit destroyed(true);
}

void AboutDialog::addText() {
    QString text;
    text.append(QString("<p><b>Zeit</b> v%1 &copy; 2015-2016 Blaze<br />"
                        "&lt;blaze@vivaldi.net&gt;</p>"
                        "<p>Qt %2 (built with Qt %3)<br />"
                        "Licensed under GPL v3 or later.</p>"
                        "<p><b>Links:</b><br />"
                        "News: <a href=\"http://loimu.tk/zeit/\">"
                        "http://loimu.tk/zeit/</a><br />"
                        "Patreon: <a href=\"https://patreon.com/blazy\">"
                        "https://patreon.com/blazy</a><br />"
                        "Project: <a href=\"https://bitbucket.org/blaze/zeit\">"
                        "https://bitbucket.org/blaze/zeit</a></p>")
                .arg(qApp->applicationVersion())
                .arg(qVersion())
                .arg(QT_VERSION_STR));
    ui->label->setText(text);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void AboutDialog::keyPressEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Escape)
        this->close();
}

void AboutDialog::on_aboutQt_released() {
    qApp->aboutQt();
}
