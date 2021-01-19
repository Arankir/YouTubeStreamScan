#include "cmd.h"

Cmd::Cmd(QObject *parent) : QObject(parent) {
    process_.start("cmd");
    process_.waitForStarted();
}

Cmd::~Cmd() {
    process_.waitForFinished();
    process_.close();
}

QString Cmd::curl(const QString &aChannel) {
    QByteArray strCommand;
    if (QSysInfo::productType() == "windows") {
        process_.write(QString("curl \"" + aChannel + "/live\" --ssl-no-revoke\n").toUtf8());
    }

    process_.waitForBytesWritten();
    process_.waitForReadyRead();

    wait(500);

    QString output = process_.readAll();
    int posStart = output.indexOf("<link rel=\"canonical\" href=\"") + 28;
    int posEnd = output.indexOf("\"", posStart + 1);
    QString answer = output.mid(posStart, posEnd - posStart);
    //qDebug() << posStart << posEnd;
    if (answer.indexOf("www.youtube.com/watch?v=") > 0) {
        //qInfo() << answer;
        //emit s_findChannel();
        return answer;
    } else {
        return "";
    }
}

QString Cmd::command(const QString &aComand) {
    if (QSysInfo::productType() == "windows") {
        process_.write(aComand.toUtf8());
    }

    process_.waitForBytesWritten();
    process_.waitForReadyRead();

    return process_.readAll();
}

void Cmd::wait(int aMSeconds) {
    QEventLoop *loop = new QEventLoop;
    QTimer *timer = new QTimer;
    timer->setInterval(aMSeconds);
    connect(timer, &QTimer::timeout, loop, &QEventLoop::quit);
    timer->start();
    loop->exec();
    delete timer;
    delete loop;
}

void Cmd::closeApp() {
    emit s_closeApp();
}
