/***************************************************************************
 *   CT Minute Header                                                      *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTMINUTE_H
#define CTMINUTE_H

#include "ctunit.h"

/**
 * Scheduled task minutes.
 */
class CTMinute : public CTUnit {
public:

	/**
	 * Constructs from a tokenized string.
	 */
	CTMinute(const QString& tokStr);

	CTMinute();
	
	int findPeriod() const;
	
	QString exportUnit() const;

};

#endif // CTMINUTE_H
