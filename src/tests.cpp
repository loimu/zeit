#include "tests.h"

int main(int argc, char *argv[])
{
    CrontabLibTests t1;
    QTest::qExec(&t1);
    return 0;
}

void CrontabLibTests::init() {
    currentTask = NULL;
    // init crontab host
    CTInitializationError error;
    ctHost = new CTHost(QLatin1String("crontab"), error);
    // init cron instance
    cron = ctHost->findCurrentUserCron();
}

void CrontabLibTests::testTokenString() {
    QString tokenString = QLatin1String("5 18 * 1 * /usr/bin/notify-send \"test\"");
    currentTask = new CTTask(tokenString,
                             QLatin1String("test task"),
                             cron->userLogin(),
                             false);
    cron->addTask(currentTask);
    cron->save();
    QString t1 = currentTask->exportTask();
    Q_ASSERT(tokenString == t1);
    cron->removeTask(currentTask);
    cron->save();
}
