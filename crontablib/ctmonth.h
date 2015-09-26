/***************************************************************************
 *   CT Month Header                                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTMONTH_H
#define CTMONTH_H

#include <QString>
#include <QList>

#include "ctunit.h"

/**
 * Scheduled task months.
 */
class CTMonth : public CTUnit {
public:

	/**
	 * Constructs from a tokenized string.
	 */
	CTMonth(const QString& tokStr = QLatin1String(""));

	/**
	 * Get natural language description.
	 */
	virtual QString describe() const;

	/**
	 * Get month name.
	 */
	static QString getName(const int ndx);

	static const int MINIMUM = 1;
	static const int MAXIMUM = 12;
private:
	static void initializeNames();
	static QList<QString> shortName;
};

#endif // CTMONTH_H
