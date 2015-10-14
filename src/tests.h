#ifndef TESTS_H_
#define TESTS_H_

#include <QtTest/QtTest>
#include <QObject>

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"
#include "ctInitializationError.h"

class CrontabLibTests: public QObject {
    Q_OBJECT

    CTHost* ctHost;
    CTCron* cron;
    CTTask* currentTask;

public:
    CrontabLibTests(QObject *parent = 0) : QObject(parent) {
    }

private slots:
    void init();
    void testTokenString();
};

#endif /* TESTS_H_ */
