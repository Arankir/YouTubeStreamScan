#ifndef STREAMLINK_H
#define STREAMLINK_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include "mainwork.h"

class StreamLink : public QObject
{
    Q_OBJECT
public:
    explicit StreamLink(const QString &aStreamLink, const QString &aChannel, const QString &aStreamUrl, const QString &aDirVideoSave, const QString &aQuality);

signals:
    void s_streamWasEnd(QString channel);

private slots:
    QString getVideoFileName(QString aUrl, const QString &aDirVideoSave);
    void streamIsEnd(int lCode, QProcess::ExitStatus lState);

private:
    QProcess *program_;
    QString channel_;
};

#endif // STREAMLINK_H
