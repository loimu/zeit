/***************************************************************************
 *   CT Environment Variable Header                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTVARIABLE_H
#define CTVARIABLE_H

#include <QString>
#include <QIcon>

/**
 * An environment variable (encapsulation of crontab environment variable
 * entry).  Encapsulates parsing and tokenization.
 */
class CTVariable {
public:

	/**
	 * Constructs environment variable from crontab format string.
	 */
	explicit CTVariable(const QString& tokenString, const QString& _comment, const QString& _userLogin);

	/**
	 * Copy constructor.
	 */
	CTVariable(const CTVariable& source);

	/**
	 * Assignment operator.
	 */
	CTVariable& operator =(const CTVariable& source);

	/**
	 * Tokenizes environment variable to crontab format.
	 */
	QString exportVariable();

	/**
	 * Mark changes as applied.
	 */
	void apply();

	/**
	 * Cancel changes.
	 */
	void cancel();

	/**
	 * Indicates whether or not the environment variable has been modified.
	 */
	bool dirty() const;
	
	QIcon variableIcon() const;
	
	QString information() const;

	QString variable;
	QString value;
	QString comment;
	QString userLogin;
	
	bool enabled;

private:
	QString initialVariable;
	QString initialValue;
	QString initialComment;
	QString initialUserLogin;
	bool initialEnabled;

};

#endif // CTVARIABLE_H
