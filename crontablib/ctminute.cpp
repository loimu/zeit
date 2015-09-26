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

#include "ctminute.h"

#include <KLocalizedString>

/**
 * Constructs from a tokenized string.
 */
CTMinute::CTMinute(const QString& tokStr) :
	CTUnit(0, 59, tokStr) {
}

CTMinute::CTMinute() :
	CTUnit(0, 59, QLatin1String( "" )) {

}

int CTMinute::findPeriod() const {
	QList<int> periods;
	periods << 1 << 2 << 5 << 10 << 15 << 20 << 30;

	return CTUnit::findPeriod(periods);
}

QString CTMinute::exportUnit() const {
	int period = findPeriod();
	if (period!=0 && period!=1)
		return QString(QLatin1String( "*/%1" )).arg(QString::number(period));

	return CTUnit::exportUnit();
}
