#ifndef MAINWORK_H
#define MAINWORK_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QDebug>
#include "cmd.h"
#include "streamlink.h"

class MainWork : public QObject
{
    Q_OBJECT
public:
    explicit MainWork(const QString &aFilePath, const QString &aStreamLink, const QString &aDirVideoSave, const QString &aQuality);
    ~MainWork();
    static bool createDir(const QString &aPath);

signals:
    void s_closeApp();

private slots:
    bool mainLoop();
    QStringList getChannelList();
    QString checkChannel(const QString &aChannel);
private:
    Cmd cmd_;

    QStringList onlineChannels_;

    QString filePath_;
    QString streamLink_;
    QString dirVideoSave_;
    QString quality_;
};

#endif // MAINWORK_H
