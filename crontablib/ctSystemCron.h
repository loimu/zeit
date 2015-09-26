/***************************************************************************
 *   CT Cron Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CT_SYSTEM_CRON_H
#define CT_SYSTEM_CRON_H

#include <QString>

#include "ctcron.h"



class CTSystemCron : public CTCron {
public:


	/**
	 * Constructs the scheduled tasks, environment variables from crontab
	 * files and obtains some information about the user from the system.
	 *
	 * Default is to construct from the user's crontab.  Can also be called,
	 * passing TRUE, to construct from the system crontab.  Throws an
	 * exception if the crontab file can not be found, read, or parsed.
	 */
	explicit CTSystemCron(const QString& cronBinary);


	/**
	 * Destructor.
	 */
	virtual ~CTSystemCron();

};

#endif // CT_SYSTEM_CRON_H
