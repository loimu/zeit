/***************************************************************************
 *   CT Day of Month Implementation                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctdom.h"

QList<QString> CTDayOfMonth::shortName;

CTDayOfMonth::CTDayOfMonth(const QString& tokStr) :
	CTUnit(CTDayOfMonth::MINIMUM, CTDayOfMonth::MAXIMUM, tokStr) {
}

QString CTDayOfMonth::describe() const {
	initializeNames();
    return (enabledCount() == CTDayOfMonth::MAXIMUM) ? tr("every day ") : CTUnit::genericDescribe(shortName);
}

QString CTDayOfMonth::getName(const int ndx) {
	initializeNames();
	return shortName[ndx];
}

void CTDayOfMonth::initializeNames() {
	if (shortName.isEmpty()) {
        shortName << QLatin1String( "" ) << tr("1st") << tr("2nd") << tr("3rd") << tr("4th") << tr("5th") << tr("6th") << tr("7th") << tr("8th") << tr("9th") << tr("10th") << tr("11th") << tr("12th") << tr("13th") << tr("14th") << tr("15th") << tr("16th") << tr("17th")
                << tr("18th") << tr("19th") << tr("20th") << tr("21st") << tr("22nd") << tr("23rd") << tr("24th") << tr("25th") << tr("26th") << tr("27th") << tr("28th") << tr("29th") << tr("30th") << tr("31st");
	}
}
