/***************************************************************************
 *   CT Host Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTHOST_H
#define CTHOST_H

#include <QString>
#include <QList>

#include "ctSaveStatus.h"

class CTTask;
class CTVariable;
class CTCron;
class CTInitializationError;

struct passwd;

/**
 * The host machine, or computer (encapsulation of crontab files on the
 * host).
 *
 * If the user is the root user, the cron vector will have a member for
 * each user of the host plus one for the system crontab.
 *
 * If the user is a non-root user, there will be only one member in the
 * cron vector.
 */
class CTHost {
public:

	/**
	 * Constructs the user(s), scheduled tasks, and environment variables
	 * from crontab files.
	 */
	CTHost(const QString& cronBinary, CTInitializationError& ctInitializationError);

	/**
	 * Destroys the user(s), scheduled tasks, and environment variable
	 * objects.  Does not make any changes to the crontab files.  Any unapplied
	 * changes are consequently "cancelled."
	 */
	~CTHost();

	/**
	 * Apply changes.
	 * return an empty string if no problem
	 */
	CTSaveStatus save();

	/**
	 * Cancel changes.
	 */
	void cancel();

	/**
	 * Indicates whether or not dirty.
	 */
	bool isDirty();
	
	/**
	 * Indicates whether or not the user is the root user.
	 */
	bool isRootUser() const;
	
	CTCron* findCurrentUserCron() const;
	CTCron* findSystemCron() const;
	CTCron* findUserCron(const QString& userLogin) const;
	
	CTCron* findCronContaining(CTTask* ctTask) const;
	CTCron* findCronContaining(CTVariable* ctVariable) const;

	/**
	 * User(s). 
	 *
	 * If the user is the root user, the cron vector will have a member for
	 * each user of the host plus one for the system crontab.
	 *
	 * If the user is a non-root user, there will be only one member in the
	 * cron vector.
	 */
	QList<CTCron*> crons;

private:

	/**
	 * Copy construction not allowed.
	 */
	CTHost(const CTHost& source);

	/**
	 * Assignment not allowed
	 */
	CTHost& operator =(const CTHost& source);

	/**
	 * Factory create a cron table.  Appends to the end of cron.
	 */
	CTCron* createSystemCron();
	CTCron* createCurrentUserCron();
	QString createCTCron(const struct passwd* password);

	/**
	 * Check /etc/cron.allow, /etc/cron.deny
	 */
	bool allowDeny(char *name);

	QString crontabBinary;
};

#endif // CTHOST_H
