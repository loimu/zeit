/***************************************************************************
 *   CT Month Implementation                                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctmonth.h"

QList<QString> CTMonth::shortName;

CTMonth::CTMonth(const QString& tokStr) :
	CTUnit(CTMonth::MINIMUM, CTMonth::MAXIMUM, tokStr) {
}

QString CTMonth::describe() const {
	initializeNames();
    return (enabledCount() == CTMonth::MAXIMUM) ? tr("every month") : CTUnit::genericDescribe(shortName);
}

QString CTMonth::getName(const int ndx) {
	initializeNames();
	return shortName[ndx];
}

void CTMonth::initializeNames() {
	if (shortName.isEmpty()) {
        shortName << QLatin1String( "" ) << tr("January") << tr("February") << tr("March") << tr("April") << tr("May", "May long") << tr("June") << tr("July", "July long") << tr("August") << tr("September") << tr("October") << tr("November") << tr("December");
	}
}
