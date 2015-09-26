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

#ifndef CT_GLOBAL_CRON_H
#define CT_GLOBAL_CRON_H

#include <QString>
#include <QList>

#include "ctcron.h"

class CTTask;
class CTVariable;
class CTHost;


/**
 * A user (encapsulation of a single crontab file).  Encapsulates
 * file i/o, parsing, tokenization, and natural language description.
 */
class CTGlobalCron : public CTCron {
public:


	explicit CTGlobalCron(CTHost* ctHost);

	/**
	 * Destructor.
	 */
	virtual ~CTGlobalCron();

	virtual QList<CTTask*> tasks() const;
	
	virtual QList<CTVariable*> variables() const;
	
	virtual void addTask(CTTask* task);
	virtual void addVariable(CTVariable* variable);
	
	virtual void modifyTask(CTTask* task);
	virtual void modifyVariable(CTVariable* variable);
		
	virtual void removeVariable(CTVariable* variable);
	virtual void removeTask(CTTask* task);

private:
	CTHost* ctHost;
};

#endif // CT_GLOBAL_CRON_H
