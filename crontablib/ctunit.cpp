/***************************************************************************
 *   CT Unit of Time Interval Implementation                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctunit.h"

#include <KLocalizedString>

#include "logging.h"

CTUnit::CTUnit(int _min, int _max, const QString& tokStr) {
	min = _min;
	max = _max;
	initialize(tokStr);
}

CTUnit::CTUnit(const CTUnit& source) {
	min = source.min;
	max = source.max;

	initialEnabled.clear();
	enabled.clear();
	for (int i = 0; i <= max; i++) {
		initialEnabled.append(false);
		enabled.append(source.enabled.at(i));
	}

	initialTokStr = QLatin1String( "" );
	dirty = true;
}

CTUnit::~CTUnit() {
}

CTUnit& CTUnit::operator = (const CTUnit& unit) {
	if (this == &unit)
		return *this;

	min = unit.min;
	max = unit.max;

	enabled.clear();
	for (int i = 0; i <= max; i++) {
		enabled.append(unit.enabled[i]);
	}
	dirty = true;

	return *this;
}

void CTUnit::initialize(const QString& tokStr) {
	enabled.clear();
	for (int i = 0; i <= max; i++) {
		enabled.append(false);
		initialEnabled.append(false);
	}

	for (int i = min; i <= max; i++) {
		initialEnabled[i] = enabled[i];
	}

	parse(tokStr);
	initialTokStr = tokStr;
	dirty = false;

}

void CTUnit::parse(const QString& tokenString) {

	QString tokStr = tokenString;

	// subelement is that which is between commas
	QString subelement;
	int commapos, slashpos, dashpos;
	int beginat, endat, step;

	// loop through each subelement
	tokStr += QLatin1Char( ',' );
	while ((commapos = tokStr.indexOf(QLatin1String( "," ))) > 0) {
		subelement = tokStr.mid(0, commapos);

		// find "/" to determine step
		slashpos = subelement.indexOf(QLatin1String( "/" ));
		if (slashpos == -1) {
			step = 1;
			slashpos = subelement.length();
		} else {
			step = fieldToValue(subelement.mid(slashpos+1, subelement.length()-slashpos-1));
			if (step < 1)
				step = 1;
		}

		// find "=" to determine range
		dashpos = subelement.indexOf(QLatin1String( "-" ));
		if (dashpos == -1) {
			// deal with "*"
			if (subelement.mid(0, slashpos) == QLatin1String( "*" )) {
				beginat = min;
				endat = max;
			} else {
				beginat = fieldToValue(subelement.mid(0, slashpos));
				endat = beginat;
			}
		} else {
			beginat = fieldToValue(subelement.mid(0, dashpos));
			endat = fieldToValue(subelement.mid(dashpos+1, slashpos-dashpos-1));
		}

		// ignore out of range
		if (beginat < 0)
			beginat = 0;
		if (endat > max)
			endat = max;

		// setup enabled
		for (int i = beginat; i <= endat; i+=step) {
			initialEnabled[i] = enabled[i] = true;
		}

		tokStr = tokStr.mid(commapos+1, tokStr.length()-commapos-1);
	}

	return;
}

QString CTUnit::exportUnit() const {
	if (!dirty) {
		return initialTokStr;
	}

	if (isAllEnabled())
		return QLatin1String( "*" );

	int total = enabledCount();
	int count = 0;
	QString tokenizeUnit;

	for (int num=min; num<=max; num++) {
		if (enabled[num]) {
			tokenizeUnit += QString::number(num);
			count++;

			if (count < total)
				tokenizeUnit += QLatin1Char( ',' );
		}
	}

	return tokenizeUnit;

}

QString CTUnit::genericDescribe(const QList<QString>& label) const {
	int total(enabledCount());
	int count(0);
	QString tmpStr;
	for (int i = min; i <= max; i++) {
		if (enabled[i]) {
			tmpStr += label.at(i);
			count++;
			switch (total - count) {
			case 0:
				break;
			case 1:
				if (total > 2)
					tmpStr += i18n(",");
				tmpStr += i18n(" and ");
				break;
			default:
				tmpStr += i18n(", ");
				break;
			}
		}
	}
	return tmpStr;
}

int CTUnit::minimum() const {
	return min;
}

int CTUnit::maximum() const {
	return max;
}

bool CTUnit::isEnabled(int pos) const {
	return enabled.at(pos);
}

bool CTUnit::isAllEnabled() const {
	for (int i = min; i <= max; i++) {
		if (enabled.at(i) == false) {
			return false;
		}
	}

	return true;
}

void CTUnit::setEnabled(int pos, bool value) {
	enabled[pos] = value;
	dirty = true;
	return;
}

bool CTUnit::isDirty() const {
	return dirty;
}

int CTUnit::enabledCount() const {
	int total(0);
	for (int i = min; i <= max; i++)
		total += (enabled[i] == true);
	return total;
}

void CTUnit::apply() {
	initialTokStr = exportUnit();
	for (int i = min; i <= max; i++)
		initialEnabled[i] = enabled[i];
	dirty = false;
	return;
}

void CTUnit::cancel() {
	for (int i = min; i <= max; i++)
		enabled[i] = initialEnabled[i];
	dirty = false;
	return;
}

int CTUnit::fieldToValue(const QString& entry) const {
	QString lower = entry.toLower();

	// check for days
	QList<QString> days;
	days << QLatin1String( "sun" ) << QLatin1String( "mon" ) << QLatin1String( "tue" ) << QLatin1String( "wed" ) << QLatin1String( "thu" ) << QLatin1String( "fri" ) << QLatin1String( "sat" );

	int day = days.indexOf(lower);
	if (day != -1) {
		return day;
	}

	// check for months
	QList<QString> months;
	months << QLatin1String( "" ) << QLatin1String( "jan" ) << QLatin1String( "feb" ) << QLatin1String( "mar" ) << QLatin1String( "apr" ) << QLatin1String( "may" ) << QLatin1String( "jun" ) << QLatin1String( "jul" ) << QLatin1String( "aug" ) << QLatin1String( "sep" ) << QLatin1String( "oct" ) << QLatin1String( "nov" ) << QLatin1String( "dec" );

	int month = months.indexOf(lower);
	if (month != -1) {
		return month;
	}

	//If the string does not match a day ora month, then it's a simple number (minute, hour or day of month)
	return entry.toInt();
}

/**
 * Find a period in enabled values
 * If no period has been found, return 0
 */
int CTUnit::findPeriod(const QList<int>& periods) const {
	foreach(int period, periods) {
		bool validPeriod = true;

		for (int i = minimum(); i <= maximum(); i++) {
			bool periodTesting;
			if ( (double)i/(double)period == i/period)
				periodTesting = true;
			else
				periodTesting = false;

			if (isEnabled(i) != periodTesting) {
				validPeriod = false;
				break;
			}
		}

		if (validPeriod) {
			return period;
		}
	}

	return 0;
}
