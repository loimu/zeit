/***************************************************************************
 *   CT Environment Variable Implementation                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctvariable.h"

#include <QRegExp>

#include <KLocalizedString>

#include "ctHelper.h"

CTVariable::CTVariable(const QString& tokenString, const QString& _comment, const QString& _userLogin) {

	QString tokStr = tokenString;

	if (tokStr.mid(0, 2) == QLatin1String( "#\\" )) {
		tokStr = tokStr.mid(2, tokStr.length() - 2);
		enabled = false;
	} else
		enabled = true;

	int spacepos(0);

	spacepos = tokStr.indexOf(QRegExp(QLatin1String( "[ =]" )));
	variable = tokStr.mid(0, spacepos);

	value = tokStr.mid(spacepos+1, tokStr.length()-spacepos-1);
	comment = _comment;

	userLogin = _userLogin;

	initialVariable = variable;
	initialValue = value;
	initialComment = comment;
	initialUserLogin = userLogin;
	initialEnabled = enabled;

}

CTVariable::CTVariable(const CTVariable &source) :
	variable(source.variable), value(source.value), comment(source.comment), userLogin(source.userLogin), enabled(source.enabled), initialVariable(QLatin1String( "" )), initialValue(QLatin1String( "" )), initialComment(QLatin1String( "" )), initialUserLogin(QLatin1String( "" )), initialEnabled(true) {
}

CTVariable& CTVariable::operator = (const CTVariable& source) {
	if (this == &source)
		return *this;

	variable = source.variable;
	value = source.value;
	comment = source.comment;
	userLogin = source.userLogin;
	enabled = source.enabled;

	initialVariable = QLatin1String( "" );
	initialValue = QLatin1String( "" );
	initialComment = QLatin1String( "" );
	initialUserLogin = QLatin1String( "" );
	initialEnabled = true;
	return *this;
}

QString CTVariable::exportVariable() {
	QString exportVariable;

	exportVariable += CTHelper::exportComment(comment);

	if (enabled == false)
		exportVariable += QLatin1String( "#\\" );

	exportVariable += variable + QLatin1String( "=" ) + value + QLatin1String( "\n" );

	return exportVariable;
}

void CTVariable::apply() {
	initialVariable = variable;
	initialValue = value;
	initialComment = comment;
	initialUserLogin = userLogin;

	initialEnabled = enabled;
}

void CTVariable::cancel() {
	variable = initialVariable;
	value = initialValue;
	comment = initialComment;
	userLogin = initialUserLogin;
	enabled = initialEnabled;
}

bool CTVariable::dirty() const {
	return ((variable != initialVariable) || (value != initialValue) || (comment != initialComment) || (userLogin != initialUserLogin) || (enabled != initialEnabled));
}

QIcon CTVariable::variableIcon() const {
	if (variable == QLatin1String( "MAILTO" ))
		return QIcon::fromTheme(QLatin1String( "mail-message" ));
	else if (variable == QLatin1String( "SHELL" ))
		return QIcon::fromTheme(QLatin1String( "utilities-terminal" ));
	else if (variable == QLatin1String( "HOME" ))
		return QIcon::fromTheme(QLatin1String( "go-home" ));
	else if (variable == QLatin1String( "PATH" ))
		return QIcon::fromTheme(QLatin1String( "folder" ));
	else if (variable == QLatin1String( "LD_CONFIG_PATH" ))
		return QIcon::fromTheme(QLatin1String( "application-x-sharedlib" ));

	return QIcon::fromTheme(QLatin1String( "text-plain" ));

}

QString CTVariable::information() const {

	if (variable == QLatin1String( "HOME" )) {
		return i18n("Override default home folder.");
	}
	else if (variable == QLatin1String( "MAILTO" )) {
		return i18n("Email output to specified account.");
	}
	else if (variable == QLatin1String( "SHELL" )) {
		return i18n("Override default shell.");
	}
	else if (variable == QLatin1String( "PATH" )) {
		return i18n("Folders to search for program files.");
	}
	else if (variable == QLatin1String( "LD_CONFIG_PATH" )) {
		return i18n("Dynamic libraries location.");
	}

	return i18n("Local Variable");

}
