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

#include "ctGlobalCron.h"


#include <kshell.h>
#include <KLocalizedString>

#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "logging.h"

CTGlobalCron::CTGlobalCron(CTHost* _ctHost) :
	CTCron() {
		
	logDebug() << "Initializing CTGlobalCron" << endl;
	
	d->multiUserCron = true;
	d->systemCron = false;
	d->currentUserCron = false;

	d->userLogin = i18n("All users");
		
	ctHost = _ctHost;
}

CTGlobalCron::~CTGlobalCron() {

}

QList<CTTask*> CTGlobalCron::tasks() const {
	logDebug() << "Global Cron Tasks" << endl;
	QList<CTTask*> tasks;
	
	foreach(CTCron* cron, ctHost->crons) {
		if (cron->isSystemCron())
			continue;
		
		foreach(CTTask* task, cron->tasks()) {
			tasks.append(task);
		}
	}
	return tasks;
}

QList<CTVariable*> CTGlobalCron::variables() const {
	logDebug() << "Global Cron Variables" << endl;
	QList<CTVariable*> variables;
	
	foreach(CTCron* cron, ctHost->crons) {
		if (cron->isSystemCron())
			continue;
		
		foreach(CTVariable* variable, cron->variables()) {
			variables.append(variable);
		}
	}
	
	return variables;
}

void CTGlobalCron::addTask(CTTask* task) {
	logDebug() << "Global Cron addTask" << endl;
	
	CTCron* actualCron = ctHost->findUserCron(task->userLogin);
	actualCron->addTask(task);
}

void CTGlobalCron::addVariable(CTVariable* variable) {
	logDebug() << "Global Cron addVariable" << endl;

	CTCron* actualCron = ctHost->findUserCron(variable->userLogin);
	actualCron->addVariable(variable);
}


void CTGlobalCron::modifyTask(CTTask* task) {
	logDebug() << "Global Cron modifyTask" << endl;
	
	CTCron* actualCron = ctHost->findCronContaining(task);
	
	/*
	 * actualCron could be NULL is the task came from clipboard because those tasks are never
	 * linked to an existing CTCron* object
	 */ 
	if (actualCron == NULL || actualCron->userLogin() != task->userLogin) {
		if (actualCron!=NULL) {
			actualCron->removeTask(task);
		}
		
		CTCron* newCron = ctHost->findUserCron(task->userLogin);
		newCron->addTask(task);
	}
}

void CTGlobalCron::modifyVariable(CTVariable* variable) {
	logDebug() << "Global Cron modifyVariable" << endl;
	
	CTCron* actualCron = ctHost->findCronContaining(variable);
	
	/*
	 * actualCron could be NULL is the task came from clipboard because those tasks are never
	 * linked to an existing CTCron* object
	 */ 
	if (actualCron == NULL || actualCron->userLogin() != variable->userLogin) {
		if (actualCron!=NULL) {
			actualCron->removeVariable(variable);
		}
		
		CTCron* newCron = ctHost->findUserCron(variable->userLogin);
		newCron->addVariable(variable);
	}
}

void CTGlobalCron::removeTask(CTTask* task) {
	logDebug() << "Global Cron removeTask" << endl;
	
	CTCron* actualCron = ctHost->findCronContaining(task);
	actualCron->removeTask(task);
}

void CTGlobalCron::removeVariable(CTVariable* variable) {
	logDebug() << "Global Cron removeVariable" << endl;
	
	CTCron* actualCron = ctHost->findCronContaining(variable);
	actualCron->removeVariable(variable);
}

