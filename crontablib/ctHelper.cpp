/***************************************************************************
 *   CT Month Implementation                                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctHelper.h"

#include <QStringList>

#include <KLocalizedString>

QString CTHelper::exportComment(const QString& comment) {
	QString exportComment;

	if (comment.isEmpty()) {
		QString noComment = i18n("No comment");
		exportComment += QLatin1String( "#" ) + noComment + QLatin1String( "\n" );
		return exportComment;
	}

	QStringList lines = comment.split(QLatin1String( "\n" ));
	foreach(const QString &line, lines) {
		exportComment += QLatin1String( "#" ) + line + QLatin1String( "\n" );
	}

	return exportComment;
}
