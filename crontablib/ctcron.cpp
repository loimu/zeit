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

#include "ctcron.h"

#include <QRegExp>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>
#include <QDateTime>

#include <kshell.h>
#include <KLocalizedString>

#include "cttask.h"
#include "ctvariable.h"
#include "ctInitializationError.h"

#include <unistd.h>      // getuid(), unlink()
#include <pwd.h>         // pwd, getpwnam(), getpwuid()

#include "logging.h"

CommandLineStatus CommandLine::execute() {
	QProcess process;

	if (standardOutputFile.isEmpty() == false)
		process.setStandardOutputFile(standardOutputFile);

	int exitCode;
	process.start(commandLine, parameters);
	if (!process.waitForStarted()) {
		exitCode = 127;
	} else {
		process.waitForFinished(-1);
		exitCode = process.exitCode();
	}

	CommandLineStatus commandLineStatus;

	commandLineStatus.commandLine = commandLine + QLatin1String( " " ) + parameters.join(QLatin1String( " " ));
	if (standardOutputFile.isEmpty() == false)
		commandLineStatus.commandLine += QLatin1String( " > " ) + standardOutputFile;

	commandLineStatus.standardOutput = QLatin1String( process.readAllStandardOutput() );
	commandLineStatus.standardError = QLatin1String( process.readAllStandardError() );
	commandLineStatus.exitCode = exitCode;

	return commandLineStatus;
}

CTCron::CTCron(const QString& crontabBinary, const struct passwd* userInfos, bool currentUserCron, CTInitializationError& ctInitializationError) :
	d(new CTCronPrivate()) {

	Q_ASSERT(userInfos != NULL);

	d->multiUserCron = false;
	d->systemCron = false;
	d->currentUserCron = currentUserCron;

	d->crontabBinary = crontabBinary;

	QTemporaryFile tmp;
	tmp.open();
	d->tmpFileName = tmp.fileName();

	CommandLine readCommandLine;

	// regular user, so provide user's own crontab
	if (currentUserCron == true) {
		readCommandLine.commandLine = d->crontabBinary;
		readCommandLine.parameters << QLatin1String( "-l" );
		readCommandLine.standardOutputFile = d->tmpFileName;

		d->writeCommandLine.commandLine = d->crontabBinary;
		d->writeCommandLine.parameters << d->tmpFileName;

	}
	else {

		readCommandLine.commandLine = d->crontabBinary;
		readCommandLine.parameters << QLatin1String( "-u" ) << QLatin1String(userInfos->pw_name) << QLatin1String( "-l" );
		readCommandLine.standardOutputFile = d->tmpFileName;

		d->writeCommandLine.commandLine = d->crontabBinary;
		d->writeCommandLine.parameters << QLatin1String( "-u" ) << QLatin1String(userInfos->pw_name) << d->tmpFileName;
	}


	d->initialTaskCount = 0;
	d->initialVariableCount = 0;

	if (initializeFromUserInfos(userInfos) == false) {
		ctInitializationError.setErrorMessage(i18n("No password entry found for uid '%1'", getuid()));
		logDebug() << "Error in crontab creation of" << userInfos->pw_name << endl;
		return;
	}

	// Don't set error if it can't be read, it means the user doesn't have a crontab.
	CommandLineStatus commandLineStatus = readCommandLine.execute();
	if (commandLineStatus.exitCode == 0) {
		this->parseFile(d->tmpFileName);
	}
	else {
		logDebug() << "Error when executing command" << commandLineStatus.commandLine << endl;
		logDebug() << "Standard output :" << commandLineStatus.standardOutput << endl;
		logDebug() << "Standard error :" << commandLineStatus.standardError << endl;
	}

	d->initialTaskCount = d->task.size();
	d->initialVariableCount = d->variable.size();
}

CTCron::CTCron() :
	d(new CTCronPrivate()) {

}

bool CTCron::initializeFromUserInfos(const struct passwd* userInfos) {
	if (userInfos == 0) {
		return false;
	} else {
		d->userLogin = QLatin1String( userInfos->pw_name );
		d->userRealName = QLatin1String( userInfos->pw_gecos );
		return true;
	}
}

CTCron& CTCron::operator = (const CTCron& source) {
	if (this == &source)
		return *this;

	if (source.isSystemCron() == true) {
		logDebug() << "Affect the system cron" << endl;
	}

	d->variable.clear();
	foreach(CTVariable* ctVariable, source.variables()) {
		CTVariable* tmp = new CTVariable(*ctVariable);
		d->variable.append(tmp);
	}

	d->task.clear();
	foreach(CTTask* ctTask, source.tasks()) {
		CTTask* tmp = new CTTask(*ctTask);
		d->task.append(tmp);
	}

	return *this;
}

void CTCron::parseFile(const QString& fileName) {

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QString comment;
	bool leadingComment = true;

	QTextStream in(&file);
	while (in.atEnd() == false) {
		QString line = in.readLine();

		// search for comments "#" but not disabled tasks "#\"
		if ( line.indexOf(QLatin1String( "#" )) == 0 && line.indexOf(QLatin1String( "\\" )) != 1 ) {
			// Skip leading comments with leading spaces, those are not written by KCron
			if ( leadingComment && line.startsWith(QLatin1String( "# " ))) {
				continue;
			}
			leadingComment = false;
			// If the first 10 characters don't contain a character, it's probably a disabled entry.
			int firstText = line.indexOf(QRegExp(QLatin1String( "\\w" )));
			if (firstText < 0)
				continue;

			if (firstText < 10) {
				// remove leading pound sign
				line = line.mid(1, line.length()-1);
				if (comment.isEmpty())
					comment = line.trimmed();
				else
					comment += QLatin1String( "\n" ) + line.trimmed();
				continue;
			}
		}

		// either a task or a variable
		int firstWhiteSpace(line.indexOf(QRegExp(QLatin1String( "[ \t]" ))));
		int firstEquals(line.indexOf(QLatin1String( "=" )));

		// if there is an equals sign and either there is no
		// whitespace or the first whitespace is after the equals
		// sign, it must be a variable
		if ((firstEquals > 0) && ((firstWhiteSpace == -1) || firstWhiteSpace > firstEquals)) {
			// create variable
			CTVariable* tmp = new CTVariable(line, comment, d->userLogin);
			d->variable.append(tmp);
			comment.clear();
		}
		// must be a task, either enabled or disabled
		else {
			if (firstWhiteSpace > 0) {
				CTTask* tmp = new CTTask(line, comment, d->userLogin, d->multiUserCron);
				d->task.append(tmp);
				comment.clear();
			}
		}



	}

}

QString CTCron::exportCron() const {
	QString exportCron;


	foreach(CTVariable* ctVariable, d->variable) {
		exportCron += ctVariable->exportVariable();
		exportCron += QLatin1String( "\n" );
	}

	foreach(CTTask* ctTask, d->task) {
		exportCron += ctTask->exportTask();
		exportCron += QLatin1String( "\n" );
	}

	exportCron += QLatin1String( "\n" );
	QString exportInfo = i18nc("Generation Message + current date", "File generated by KCron the %1.", QDateTime::currentDateTime().toString(QLocale().dateTimeFormat()));
	exportCron += QLatin1String( "# " ) + exportInfo + QLatin1String( "\n" );

	return exportCron;
}

CTCron::~CTCron() {
	foreach(CTTask* ctTask, d->task) {
		delete ctTask;
	}

	foreach(CTVariable* ctVariable, d->variable) {
		delete ctVariable;
	}

	delete d;
}

bool CTCron::saveToFile(const QString& fileName) {
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}

	//logDebug() << exportCron() << endl;

	QTextStream out(&file);
	out << exportCron();

	out.flush();
	file.close();

	return true;
}

CTSaveStatus CTCron::prepareSaveStatusError(const CommandLineStatus& commandLineStatus) {
	QString standardOutput;
	if (commandLineStatus.standardOutput.isEmpty())
		standardOutput = i18n("<em>No output.</em>");
	else
		standardOutput = commandLineStatus.standardOutput;

	QString standardError;
	if (commandLineStatus.standardError.isEmpty())
		standardError = i18n("<em>No error.</em>");
	else
		standardError = commandLineStatus.standardError;

	QString detailError;
	if (commandLineStatus.exitCode == 127)
		detailError = i18n("<p><strong>Command:</strong> %1</p><strong>Command could not be started</strong>", commandLineStatus.commandLine);
	else
		detailError = i18n("<p><strong>Command:</strong> %1</p><strong>Standard Output :</strong><pre>%2</pre><strong>Error Output :</strong><pre>%3</pre>", commandLineStatus.commandLine, standardOutput, standardError);

	return CTSaveStatus(i18n("An error occurred while updating crontab."), detailError);
}

CTSaveStatus CTCron::save() {
	// write to temp file
	bool saveStatus = saveToFile(d->tmpFileName);
	if (saveStatus == false) {
		return CTSaveStatus(i18n("Unable to open crontab file for writing"), i18n("The file %1 could not be opened.", d->tmpFileName));
	}

	CommandLineStatus commandLineStatus = d->writeCommandLine.execute();
	// install temp file into crontab
	if (commandLineStatus.exitCode != 0) {
		QFile::remove(d->tmpFileName);
		return prepareSaveStatusError(commandLineStatus);
	}
	else {
		//Remove the temp file
		QFile::remove(d->tmpFileName);
	}


	//Mark as applied
	foreach(CTTask* ctTask, d->task) {
		ctTask->apply();
	}

	foreach(CTVariable* ctVariable, d->variable) {
		ctVariable->apply();
	}

	d->initialTaskCount = d->task.size();
	d->initialVariableCount = d->variable.size();

	return CTSaveStatus();
}

void CTCron::cancel() {
	foreach(CTTask* ctTask, d->task) {
		ctTask->cancel();
	}

	foreach(CTVariable* ctVariable, d->variable) {
		ctVariable->cancel();
	}

}

bool CTCron::isDirty() const {
	if (d->initialTaskCount != d->task.count())
		return true;

	if (d->initialVariableCount != d->variable.count())
		return true;

	foreach(CTTask* ctTask, d->task) {
		if (ctTask->dirty())
		return true;
	}

	foreach(CTVariable* ctVariable, d->variable) {
		if (ctVariable->dirty())
		return true;
	}

	return false;
}

QString CTCron::path() const {
	QString path;

	foreach(CTVariable* ctVariable, d->variable) {
		if (ctVariable->variable == QLatin1String( "PATH" )) {
			path = ctVariable->value;
		}
	}

	return path;

}

QList<CTTask*> CTCron::tasks() const {
	return d->task;
}

QList<CTVariable*> CTCron::variables() const {
	return d->variable;
}

void CTCron::addTask(CTTask* task) {
	if (isSystemCron()) {
		task->setSystemCrontab(true);
	}
	else {
		task->userLogin = d->userLogin;
		task->setSystemCrontab(false);
	}

	logDebug() << "Adding task" << task->comment << " user : "<< task->userLogin << endl;

	d->task.append(task);
}

void CTCron::addVariable(CTVariable* variable) {
	if (isSystemCron())
		variable->userLogin = QLatin1String( "root" );
	else
		variable->userLogin = d->userLogin;


	logDebug() << "Adding variable" << variable->variable << " user : "<< variable->userLogin << endl;

	d->variable.append(variable);
}


void CTCron::modifyTask(CTTask* /*task*/) {
	//Nothing to do specifically when modifying a task
}

void CTCron::modifyVariable(CTVariable* /*variable*/) {
	//Nothing to do specifically when modifying a variable
}

void CTCron::removeTask(CTTask* task) {
	d->task.removeAll(task);
}

void CTCron::removeVariable(CTVariable* variable) {
	d->variable.removeAll(variable);
}


bool CTCron::isMultiUserCron() const {
	return d->multiUserCron;
}

bool CTCron::isCurrentUserCron() const {
	return d->currentUserCron;
}

bool CTCron::isSystemCron() const {
	return d->systemCron;
}


QString CTCron::userLogin() const {
	return d->userLogin;
}

QString CTCron::userRealName() const {
	return d->userRealName;
}


