/***************************************************************************
 *   CT Day Of Week Implementation                                         *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctdow.h"

QList<QString> CTDayOfWeek::shortName;

QList<QString> CTDayOfWeek::longName;

CTDayOfWeek::CTDayOfWeek(const QString& tokStr) :
	CTUnit(CTDayOfWeek::MINIMUM, CTDayOfWeek::MAXIMUM, tokStr) {
	// Compensate for cron supporting Sunday as both 0 and 7.

	if (isEnabled(0)) {
		setEnabled(0, false);
		setEnabled(7, true);
	}
}

void CTDayOfWeek::initialize(const QString &tokStr) {
	CTUnit::initialize(tokStr);

	// Compensate for cron supporting Sunday as both 0 and 7.

	if (isEnabled(0)) {
		setEnabled(0, false);
		setEnabled(7, true);
		apply();
	}
}

QString CTDayOfWeek::describe() const {
	initializeNames();
	if (enabledCount() == CTDayOfWeek::MAXIMUM)
        return tr("every day ");
	else
		return CTUnit::genericDescribe(shortName);
}

QString CTDayOfWeek::getName(const int ndx, const bool format) {
	initializeNames();
	return (format == shortFormat) ? shortName[ndx] : longName[ndx];
}

void CTDayOfWeek::initializeNames() {
	if (shortName.isEmpty()) {
        shortName << QLatin1String( "" ) << tr("Mon") << tr("Tue") << tr("Wed") << tr("Thu") << tr("Fri") << tr("Sat") << tr("Sun");

        longName << QLatin1String( "" ) << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday") << tr("Friday") << tr("Saturday") << tr("Sunday");

	}
}
