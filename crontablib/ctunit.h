/***************************************************************************
 *   CT Unit of Time Interval Header                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTUNIT_H
#define CTUNIT_H

#include <QString>
#include <QList>

/**
 * A cron table unit parser and tokenizer.
 * Parses/tokenizes unit such as "0-3,5,6,10-30/5"
 * Also provides default natural language description.
 */
class CTUnit {
	
protected:
	CTUnit(int min, int max, const QString& tokStr = QLatin1String(""));

	/**
	 * Get default natural language description.
	 */
	virtual QString genericDescribe(const QList<QString>& label) const;
	
public:

	/**
	 * Base initial image as empty and copy enabled intervals.
	 */
	CTUnit(const CTUnit& source);

	/**
	 * Destructor.
	 */
	virtual ~CTUnit();

	/**
	 * Copy enabled intervals.
	 */
	CTUnit& operator =(const CTUnit& unit);

	/**
	 * Tokenizes unit into string such as
	 * "0,1,2,3,5,6,10,15,20,25,30".
	 */
	virtual QString exportUnit() const;

	/**
	 * Parses unit such as "0-3,5,6,10-30/5".
	 * And initialize array of enabled intervals.
	 */
	void initialize(const QString& tokStr = QLatin1String(""));

	/**
	 * Lower bound.
	 */
	int minimum() const;

	/**
	 * Upper bound.
	 */
	int maximum() const;

	/**
	 * Accessor.
	 */
	bool isEnabled(int pos) const;

	bool isAllEnabled() const;

	void setEnabled(int pos, bool value);

	/**
	 * Indicates whether enabled intervals have been modified.
	 */
	bool isDirty() const;

	/**
	 * Total count of enabled intervals.
	 */
	int enabledCount() const;

	/**
	 * Mark changes as applied.
	 */
	void apply();

	/**
	 * Mark cancel changes and revert to initial or last applied
	 * image.
	 */
	void cancel();

	/**
	 * Find a period in enabled values
	 * If no period has been found, return 0
	 */
	int findPeriod(const QList<int>& periods) const;

protected:	

	/**
	 * Parses unit such as "0-3,5,6,10-30/5".
	 * Does not initialize array of enabled intervals.
	 */
	void parse(const QString& tokenString = QLatin1String(""));
	
private:
	int min;
	int max;

	int fieldToValue(const QString& entry) const;
	bool dirty;
	
	QList<bool> enabled;
	QList<bool> initialEnabled;
	
	QString initialTokStr;

public:

	/**
	 * Constant indicating short format.
	 */
	static const bool shortFormat = 0;

	/**
	 * Constant indicating long format.
	 */
	static const bool longFormat = 1;

};

#endif // CTUNIT_H
