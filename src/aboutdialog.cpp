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
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : BaseDialog(parent)
{
    this->setWindowTitle(tr("About Zeit"));
    auto* horizontalLayout = new QHBoxLayout(this);
    auto* label = new QLabel(this);
    label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    horizontalLayout->addWidget(label);
    auto* verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);
    verticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
    auto* iconLabel = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(iconLabel->sizePolicy().hasHeightForWidth());
    iconLabel->setSizePolicy(sizePolicy);
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/zeit.png")));
    iconLabel->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);
    verticalLayout->addWidget(iconLabel);
    auto* aboutQt = new QPushButton(this);
    aboutQt->setText(tr("About &Qt"));
    connect(aboutQt, &QPushButton::released, qApp, &QApplication::aboutQt);
    verticalLayout->addWidget(aboutQt);
    auto* buttonBox = new QDialogButtonBox(this);
    buttonBox->setMaximumSize(QSize(128, 16777215));
    buttonBox->setOrientation(Qt::Vertical);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    buttonBox->setCenterButtons(false);
    verticalLayout->addWidget(buttonBox);
    horizontalLayout->addLayout(verticalLayout);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AboutDialog::close);
    label->setText(
                QString(
                    QLatin1String(
                        "<p><b>Zeit</b> v%1 &copy; 2015-2021 Blaze<br />"
                        "&lt;blaze@vivaldi.net&gt;</p>"
                        "<p>Qt %2 (built with Qt %3)<br />"
                        "Licensed under GPL v3 or later</p>"
                        "<p>Built on %4</p>Project:<br />"
                        "<a href=\"https://launchpad.net/zeit\">"
                        "https://launchpad.net/zeit</a>"))
                .arg(qApp->applicationVersion(), qVersion(), QT_VERSION_STR, __DATE__));
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}
