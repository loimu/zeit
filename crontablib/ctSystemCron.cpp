/***************************************************************************
 *   CT Cron Implementation                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctSystemCron.h"


#include <kshell.h>
#include <KLocalizedString>

#include <QTemporaryFile>

#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "logging.h"

CTSystemCron::CTSystemCron(const QString& crontabBinary) :
	CTCron() {

	d->systemCron = true;
	d->multiUserCron = true;
	d->currentUserCron = false;

	d->crontabBinary = crontabBinary;

	QTemporaryFile tmp;
	tmp.open();
	d->tmpFileName = tmp.fileName();

	CommandLine readCommandLine;

	readCommandLine.commandLine = QLatin1String( "cat" );
	readCommandLine.parameters << QLatin1String( "/etc/crontab" );
	readCommandLine.standardOutputFile = d->tmpFileName;

	d->writeCommandLine.commandLine = QLatin1String( "cat" );
	d->writeCommandLine.parameters << d->tmpFileName;
	d->writeCommandLine.standardOutputFile = QLatin1String( "/etc/crontab" );

	d->userLogin = i18n("System Crontab");
	d->userRealName = d->userLogin;

	d->initialTaskCount = 0;
	d->initialVariableCount = 0;

	// Don't set error if it can't be read, it means the user
	// doesn't have a crontab.
	if (readCommandLine.execute().exitCode == 0) {
		this->parseFile(d->tmpFileName);
	}

	d->initialTaskCount = d->task.size();
	d->initialVariableCount = d->variable.size();
}


CTSystemCron::~CTSystemCron() {

}
