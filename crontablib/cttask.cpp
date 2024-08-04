/***************************************************************************
 *   CT Task Implementation                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cttask.h"

#include <QRegularExpression>
#include <QMimeDatabase>
#include <QUrl>

#include "ctHelper.h"

#include "logging.h"

CTTask::CTTask(const QString& tokenString, const QString& _comment,
               const QString& _userLogin, bool _systemCrontab) :
    systemCrontab(_systemCrontab) {

    QString tokStr = tokenString;
    if (tokStr.mid(0, 2) == QLatin1String( "#\\" )) {
        tokStr = tokStr.mid(2, tokStr.length() - 2);
        enabled = false;
    } else if (tokStr.mid(0, 1) == QChar::fromLatin1('#')) {
        tokStr = tokStr.mid(1, tokStr.length() - 1);
        enabled = false;
    } else
        enabled = true;

    // Skip over 'silence' if found... old option in vixie cron
    if (tokStr.mid(0, 1) == QChar::fromLatin1('-'))
        tokStr = tokStr.mid(1, tokStr.length() - 1);

    reboot = false;
    if (tokStr.mid(0, 1) == QChar::fromLatin1('@')) {
        if (tokStr.mid(1, 6) == QLatin1String( "yearly" )) {
            tokStr = QLatin1String( "0 0 1 1 *" )+tokStr.mid(7, tokStr.length()-1);
        } else if (tokStr.mid(1, 8) == QLatin1String( "annually" )) {
            tokStr = QLatin1String( "0 0 1 1 *" )+tokStr.mid(9, tokStr.length()-1);
        } else if (tokStr.mid(1, 7) == QLatin1String( "monthly" )) {
            tokStr = QLatin1String( "0 0 1 * *" )+tokStr.mid(8, tokStr.length()-1);
        } else if (tokStr.mid(1, 6) == QLatin1String( "weekly" )) {
            tokStr = QLatin1String( "0 0 * * 0" )+tokStr.mid(7, tokStr.length()-1);
        } else if (tokStr.mid(1, 5) == QLatin1String( "daily" )) {
            tokStr = QLatin1String( "0 0 * * *" )+tokStr.mid(6, tokStr.length()-1);
        } else if (tokStr.mid(1, 6) == QLatin1String( "hourly" )) {
            tokStr = QLatin1String( "0 * * * *" )+tokStr.mid(7, tokStr.length()-1);
        } else if (tokStr.mid(1, 6) == QLatin1String( "reboot" )) {
            tokStr = tokStr.mid(7, tokStr.length()-1);
            reboot = true;
        }
    }

    static const QRegularExpression spacePosRx(QStringLiteral("[ \t]"));
    int spacePos(tokStr.indexOf(spacePosRx));
    // If reboot bypass initialize functions so no keys selected in modify task
    if (reboot == false) {

        //logDebug() << "Line : " << tokStr << endl;
        minute.initialize(tokStr.mid(0, spacePos));

        while (isSpace(tokStr, spacePos+1))
            spacePos++;
        tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
        spacePos = tokStr.indexOf(spacePosRx);
        hour.initialize(tokStr.mid(0, spacePos));

        while (isSpace(tokStr, spacePos+1))
            spacePos++;
        tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
        spacePos = tokStr.indexOf(spacePosRx);
        dayOfMonth.initialize(tokStr.mid(0, spacePos));

        while (isSpace(tokStr, spacePos+1))
            spacePos++;
        tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
        spacePos = tokStr.indexOf(spacePosRx);
        month.initialize(tokStr.mid(0, spacePos));

        while (isSpace(tokStr, spacePos+1))
            spacePos++;
        tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
        spacePos = tokStr.indexOf(spacePosRx);
        dayOfWeek.initialize(tokStr.mid(0, spacePos));
    }

    if (systemCrontab) {
        while (isSpace(tokStr, spacePos+1))
            spacePos++;
        tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
        spacePos = tokStr.indexOf(spacePosRx);
        userLogin = tokStr.mid(0, spacePos);
    }
    else {
        userLogin = _userLogin;
    }

    command = tokStr.mid(spacePos+1, tokStr.length()-1);
    // remove leading whitespace
    while (command.indexOf(spacePosRx) == 0)
        command = command.mid(1, command.length()-1);
    comment = _comment;

    initialUserLogin = userLogin;
    initialCommand = command;
    initialComment = comment;
    initialEnabled = enabled;
    initialReboot = reboot;
}

CTTask::CTTask(const CTTask &source) :
    month(source.month), dayOfMonth(source.dayOfMonth),
    dayOfWeek(source.dayOfWeek), hour(source.hour), minute(source.minute),
    userLogin(source.userLogin),command(source.command),comment(source.comment),
    enabled(source.enabled), reboot(source.reboot)
{

}

CTTask& CTTask::operator=(const CTTask& source) {
    if (this == &source)
        return *this;

    month = source.month;
    dayOfMonth = source.dayOfMonth;
    dayOfWeek = source.dayOfWeek;
    hour = source.hour;
    minute = source.minute;
    userLogin = source.userLogin;
    command = source.command;
    comment = source.comment;
    enabled = source.enabled;
    reboot = source.reboot;
    initialUserLogin = QString();
    initialCommand = QString();
    initialComment = QString();
    initialEnabled = true;
    initialReboot = false;

    return *this;
}

QString CTTask::exportTask() {
    QString exportTask;
    exportTask += CTHelper::exportComment(comment);
    if (enabled == false)
        exportTask += QLatin1String( "#\\" );
    exportTask += schedulingCronFormat();
    exportTask += QChar::fromLatin1('\t');
    if (isSystemCrontab() == true)
        exportTask += userLogin + QChar::fromLatin1('\t');
    exportTask += command + QChar::fromLatin1('\n');
    return exportTask;
}

void CTTask::apply() {
    month.apply();
    dayOfMonth.apply();
    dayOfWeek.apply();
    hour.apply();
    minute.apply();

    initialUserLogin = userLogin;
    initialCommand = command;
    initialComment = comment;
    initialEnabled = enabled;
    initialReboot = reboot;
}

void CTTask::cancel() {
    month.cancel();
    dayOfMonth.cancel();
    dayOfWeek.cancel();
    hour.cancel();
    minute.cancel();

    userLogin = initialUserLogin;
    command = initialCommand;
    comment = initialComment;
    enabled = initialEnabled;
    reboot = initialReboot;
}

bool CTTask::dirty() const {
    return (month.isDirty() || dayOfMonth.isDirty() || dayOfWeek.isDirty() || hour.isDirty() || minute.isDirty() || (userLogin != initialUserLogin) || (command != initialCommand) || (comment != initialComment) || (enabled != initialEnabled) || (reboot != initialReboot));
}

QString CTTask::schedulingCronFormat() const {
    if (reboot)
        return QStringLiteral("@reboot");
    QString scheduling;
    scheduling += minute.exportUnit();
    scheduling += QChar(0x20) + hour.exportUnit();
    scheduling += QChar(0x20) + dayOfMonth.exportUnit();
    scheduling += QChar(0x20) + month.exportUnit();
    scheduling += QChar(0x20) + dayOfWeek.exportUnit();
    return scheduling;
}

/**
 * Of the whole program, this method is probably the trickiest.
 *
 * This method creates the natural language description, such as
 * "At 1:00am, every Sun".
 *
 * First, I declare some strings for holding what can be internationalized.
 * Note the tokens such as "MONTHS".  Translators should reuse these
 * tokens in their translations.  See README.translators for more
 * information.
 *
 * Second, I get the descriptions from the component parts such as
 * days of the month.
 *
 * Third, I get hour/minute time combinations.  Although a little bit
 * awkward, I use the tm struct and strftime from <time.h>.  This
 * way this code is portable across all Unixes.
 *
 * Fourth, I know that "every day of the week" and "every day of the
 * month" simply makes "every day".
 *
 * Fifth and finally I do tag substitution to create the natural language
 * description.
 *
 */
QString CTTask::describe() const {

    if (reboot) {
        return tr("at system startup");
    }

    QString dateFormat = createDateFormat();

    QString timeFormat = createTimeFormat();

    return tr("%1, %2", "1:Time Description, 2:Date Description").arg(
                timeFormat, dateFormat);
}

QString CTTask::describeDayOfWeek() const {
    return tr("every %1", "Every 'days of week'").arg(dayOfWeek.describe());
}

QString CTTask::describeDayOfMonth() const {
    return tr("%1 of %2", "'Days of month' of 'Months'").arg(
                dayOfMonth.describe(), month.describe());
}

QString CTTask::createDateFormat() const {
    /*
     * "* * *" means truly every day.
     * Note: Languages may use different phrases to indicate
     * every day of month versus every day of week.
     */
    QString dateFormat;
    if ((dayOfMonth.enabledCount() == CTDayOfMonth::MAXIMUM)
            && (dayOfWeek.enabledCount() == CTDayOfWeek::MAXIMUM)) {
        dateFormat = tr("every day ");
    }
    // Day of month not specified, so use day of week.
    else if (dayOfMonth.enabledCount() == CTDayOfMonth::MAXIMUM) {
        dateFormat = describeDayOfWeek();
    }
    // Day of week not specified, so use day of month.
    else if (dayOfWeek.enabledCount() == CTDayOfWeek::MAXIMUM) {
        dateFormat = describeDayOfMonth();
    }
    else {
        dateFormat = tr("%1 as well as %2",
                        "1:Day of month, 2:Day of week").arg(
                    describeDayOfMonth(), describeDayOfWeek());
    }

    return dateFormat;

}

QString CTTask::describeDateAndHours() const {
    // Create time description.
    int total = minute.enabledCount() * hour.enabledCount();

    QString timeDesc;
    int count = 0;

    for (int h = 0; h <= 23; h++) {
        if (hour.isEnabled(h)) {
            for (int m = 0; m <= 59; m++) {
                if (minute.isEnabled(m) == true) {
                    QString hourString;
                    if (h<10)
                        hourString = QChar::fromLatin1('0') + QString::number(h);
                    else
                        hourString = QString::number(h);

                    QString minuteString;
                    if (m<10)
                        minuteString = QChar::fromLatin1('0') + QString::number(m);
                    else
                        minuteString = QString::number(m);

                    QString tmpStr = tr("%1:%2", "1:Hour, 2:Minute").arg(
                                hourString, minuteString);

                    timeDesc += tmpStr;
                    count++;
                    switch (total - count) {
                    case 0:
                        break;
                    case 1:
                        if (total > 2)
                            timeDesc += tr(", and ");
                        else
                            timeDesc += tr(" and ");
                        break;
                    default:
                        timeDesc += tr(", ");
                    }
                }
            }
        }
    }
    return tr("at %1", "Hour::Minute list").arg(timeDesc);
}

QString CTTask::createTimeFormat() const {
    if (hour.isAllEnabled()) {
        int minutePeriod = minute.findPeriod();
        if (minutePeriod != 0)
            return tr("Every minute", "Every minute");
    }

    return describeDateAndHours();
}

bool CTTask::isSystemCrontab() const {
    return systemCrontab;
}

void CTTask::setSystemCrontab(bool _systemCrontab) {
    systemCrontab = _systemCrontab;
}


QIcon CTTask::commandIcon() const {
    QUrl commandPath = QUrl::fromLocalFile(completeCommandPath());

    QMimeType mimeType = QMimeDatabase().mimeTypeForUrl(commandPath);
    //logDebug() << mimeType->name() << endl;
    if (mimeType.name() == QLatin1String( "application/x-executable" )
            || mimeType.name() == QLatin1String( "application/octet-stream" )) {

        //The next line is identical as SmallIcon(commandPath.fileName()), but is able to return a isNull() QPixmap
        //QPixmap pixmap = KIconLoader::global()->loadIcon(commandPath.fileName(), KIconLoader::Small, 0, KIconLoader::DefaultState, QStringList(), 0L, true);
        //if (pixmap.isNull()) {
            return QIcon::fromTheme(QLatin1String("system-run"));
        //}

        //return QIcon(pixmap);
    }

    return QIcon::fromTheme(mimeType.iconName());

}

QPair<QString, bool> CTTask::unQuoteCommand() const {
    QString fullCommand = command;
    fullCommand = fullCommand.trimmed();

    const QVector<QChar> quotes {
        QChar::fromLatin1('"'), QChar::fromLatin1('\'') };
    for(const QChar &quote: quotes) {
        if (fullCommand.indexOf(quote) == 0) {
            int nextQuote = fullCommand.indexOf(quote, 1);
            if (nextQuote == -1)
                return QPair<QString, bool>(QString(), false);

            return QPair<QString, bool>(fullCommand.mid(1, nextQuote-1), true);
        }

    }

    return QPair<QString, bool>(fullCommand, false);

}

QString CTTask::decryptBinaryCommand(const QString& command) const {
    QString fullCommand;
    bool found = false;
    for(int i=0; i < command.length(); ++i) {
        if(command.at(i) == QChar(0x20)
                && command.at(i-1) != QChar::fromLatin1('\\')) {
            fullCommand = command.left(i);
            found = true;
            break;
        }
    }
    if (found == false)
        fullCommand = command;
    fullCommand = fullCommand.remove(QChar::fromLatin1('\\'));
    return fullCommand;
}

QStringList CTTask::separatePathCommand(const QString& command, bool quoted) const {
    QStringList pathCommand;
    if (command.at(0) == QChar::fromLatin1('/')) {
        QString fullCommand;
        if (quoted == true)
            fullCommand = command;
        else
            fullCommand = decryptBinaryCommand(command);

        if (fullCommand.isEmpty()) {
            return QStringList();
        }

        QString path = fullCommand.section(QChar::fromLatin1('/'), 0, -2);
        QString commandBinary = fullCommand.section(QChar::fromLatin1('/'), -1);
        pathCommand << path << commandBinary;
    } else {
        QString fullCommand;
        if (quoted == true)
            fullCommand = command;
        else
            fullCommand = decryptBinaryCommand(command);
        // relying on $PATH
        pathCommand << QString() << fullCommand;
    }
    return pathCommand;
}

QString CTTask::completeCommandPath() const {
    QPair<QString, bool> commandQuoted = unQuoteCommand();
    if (commandQuoted.first.isEmpty())
        return QString();
    QStringList pathCommand = separatePathCommand(
                commandQuoted.first, commandQuoted.second);
    if (pathCommand.isEmpty()) {
        return QString();
    }
    return pathCommand.join(QChar::fromLatin1('/'));
}
