/***************************************************************************
 *   CT Day of Month Header                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTDOM_H
#define CTDOM_H

#include <QString>
#include <QList>

#include "ctunit.h"

/**
 * Scheduled task days of month.
 */
class CTDayOfMonth : public CTUnit {
public:
	/**
	 * Constructs from a tokenized string.
	 */
	CTDayOfMonth(const QString& tokStr = QLatin1String(""));

	/**
	 * Get natural language description.
	 */
	virtual QString describe() const;

	/**
	 * Get day of month name.
	 */
	static QString getName(const int ndx);

	static const int MINIMUM = 1;
	static const int MAXIMUM = 31;
private:
	static void initializeNames();
	static QList<QString> shortName;
};

#endif // CTDOM_H
