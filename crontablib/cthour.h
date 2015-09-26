/***************************************************************************
 *   CT Hour Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTHOUR_H
#define CTHOUR_H

#include "ctunit.h"

/**
 * Scheduled task hours.
 */
class CTHour : public CTUnit {
public:

	/**
	 * Constructs from a tokenized string.
	 */
	CTHour(const QString& tokStr = QLatin1String(""));
	
	int findPeriod() const;
	
	QString exportUnit() const;
};

#endif // CTHOUR_H
