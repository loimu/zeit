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
#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "logging.h"

CTGlobalCron::CTGlobalCron(CTHost* _ctHost) :
	CTCron() {
		
    logDebug() << "Initializing CTGlobalCron";
	
	d->multiUserCron = true;
	d->systemCron = false;
	d->currentUserCron = false;

    d->userLogin = tr("All users");
		
	ctHost = _ctHost;
}

CTGlobalCron::~CTGlobalCron() {

}

const QList<CTTask*> CTGlobalCron::tasks() const {
    logDebug() << "Global Cron Tasks";
	QList<CTTask*> tasks;
	
    for(CTCron* cron: ctHost->cronInstances) {
		if (cron->isSystemCron())
			continue;
		
        for(CTTask* task: cron->tasks()) {
			tasks.append(task);
		}
	}
	return tasks;
}

const QList<CTVariable*> CTGlobalCron::variables() const {
    logDebug() << "Global Cron Variables";
	QList<CTVariable*> variables;
	
    for(CTCron* cron: ctHost->cronInstances) {
		if (cron->isSystemCron())
			continue;
		
        for(CTVariable* variable: cron->variables()) {
			variables.append(variable);
		}
	}
	
	return variables;
}

void CTGlobalCron::addTask(CTTask* task) {
    logDebug() << "Global Cron addTask";
	
	CTCron* actualCron = ctHost->findUserCron(task->userLogin);
	actualCron->addTask(task);
}

void CTGlobalCron::addVariable(CTVariable* variable) {
    logDebug() << "Global Cron addVariable";

	CTCron* actualCron = ctHost->findUserCron(variable->userLogin);
	actualCron->addVariable(variable);
}


void CTGlobalCron::modifyTask(CTTask* task) {
    logDebug() << "Global Cron modifyTask";
	
	CTCron* actualCron = ctHost->findCronContaining(task);
	
	/*
	 * actualCron could be NULL is the task came from clipboard because those tasks are never
	 * linked to an existing CTCron* object
	 */ 
    if (actualCron == nullptr || actualCron->userLogin() != task->userLogin) {
        if (actualCron!=nullptr) {
			actualCron->removeTask(task);
		}
		
		CTCron* newCron = ctHost->findUserCron(task->userLogin);
		newCron->addTask(task);
	}
}

void CTGlobalCron::modifyVariable(CTVariable* variable) {
    logDebug() << "Global Cron modifyVariable";
	
	CTCron* actualCron = ctHost->findCronContaining(variable);
	
	/*
	 * actualCron could be NULL is the task came from clipboard because those tasks are never
	 * linked to an existing CTCron* object
	 */ 
    if (actualCron == nullptr || actualCron->userLogin() != variable->userLogin) {
        if (actualCron!=nullptr) {
			actualCron->removeVariable(variable);
		}
		
		CTCron* newCron = ctHost->findUserCron(variable->userLogin);
		newCron->addVariable(variable);
	}
}

void CTGlobalCron::removeTask(CTTask* task) {
    logDebug() << "Global Cron removeTask";
	
	CTCron* actualCron = ctHost->findCronContaining(task);
	actualCron->removeTask(task);
}

void CTGlobalCron::removeVariable(CTVariable* variable) {
    logDebug() << "Global Cron removeVariable";
	
	CTCron* actualCron = ctHost->findCronContaining(variable);
	actualCron->removeVariable(variable);
}

