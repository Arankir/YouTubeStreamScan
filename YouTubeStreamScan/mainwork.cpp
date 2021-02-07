#include "mainwork.h"

MainWork::MainWork(const QString &aFilePath, const QString &aStreamLink, const QString &aDirVideoSave, const QString &aQuality):
    QObject(), filePath_(aFilePath), streamLink_(aStreamLink), dirVideoSave_(aDirVideoSave), quality_(aQuality) {
    qInfo() << "start scanning";
    mainLoop();
}

MainWork::~MainWork() {
    qInfo() << "scanning is end";
    emit s_closeApp();
}

bool MainWork::mainLoop() {
    while (true) {
        QStringList list = getChannelList();
        if (list.count() == 0) {
            return false;
        }
        for (const QString &channel: list) {
            QString streamUrl = checkChannel(channel);
            if (streamUrl != "") {
                qInfo() << "url is finded:" << streamUrl;
                onlineChannels_.append(cmd_.secondChannel());
                StreamLink *stream = new StreamLink(streamLink_, cmd_.secondChannel(), streamUrl, dirVideoSave_, quality_);
                connect(stream, &StreamLink::s_streamWasEnd, this, [=](QString channel) {
                    onlineChannels_.removeOne(channel);
                });
                //openStreamLink(streamUrl, getVideoFileName(streamUrl), cmd_.lastChannel());
            }
        }
        Cmd::wait(10000);
    }
    return true;
}

QStringList MainWork::getChannelList() {
    QFile file(filePath_);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "file with channels is not available";
        return QStringList();
    }
    QStringList list;
    while (!file.atEnd()) {
        list << file.readLine();
    }
    file.close();
    return list;
}

QString MainWork::checkChannel(const QString &aChannel) {
    return cmd_.curl(aChannel);
}

bool MainWork::createDir(const QString &aPath) {
    bool exist = true;
    QString path = aPath;
    path.replace("\\", "/");
    QStringList dirs = path.split("/");
    if (dirs.last() != "") {
        dirs.removeLast();
    }
    QString pathNow = "";
    for (auto &dir: dirs) {
        pathNow += std::move(dir) + "/";
        exist = (QDir().mkdir(pathNow) && exist);
    }
    return exist;
}
