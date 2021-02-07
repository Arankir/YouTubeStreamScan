#include "streamlink.h"

StreamLink::StreamLink(const QString &aStreamLink, const QString &aChannel, const QString &aStreamUrl, const QString &aDirVideoSave, const QString &aQuality) : QObject() {
    QString fileName = getVideoFileName(aStreamUrl, aDirVideoSave);
    QProcess *program = new QProcess;
    program->start(aStreamLink, QStringList() << aStreamUrl << aQuality << fileName << "--subprocess-errorlog" << "--subprocess-errorlog-path ./logs" << "-o");
    program->waitForStarted();
    qInfo() << "streamLink was started with channel " + aChannel;
    connect(program, SIGNAL(QProcess::finished(int lCode, QProcess::ExitStatus lState)), this, SLOT([=](int lCode, QProcess::ExitStatus lState) {
        if (lState != QProcess::ExitStatus::NormalExit) {
            qWarning() << "last error is" << program->error();
        }
        qInfo() << "program was finished with channel " + aChannel + " (" + QString::number(lCode) + ")";

        emit s_streamWasEnd(aChannel);
        //onlineChannels_.removeOne(aChannel);
        deleteLater();
    }));
}

QString StreamLink::getVideoFileName(QString aUrl, const QString &aDirVideoSave) {
    QString fileName = aUrl.remove("https://").remove("www.").remove("youtube.com/").remove("watch?v=");
    MainWork::createDir(aDirVideoSave);
    int fileIndex = 0;

    while(QFile::exists(aDirVideoSave + fileName + ".mp4")) {
        fileName = aUrl + "_" + QString::number(++fileIndex);
    }
    qInfo() << "video path:" << aDirVideoSave + fileName + ".mp4";
    return aDirVideoSave + fileName + ".mp4";
}

//void openStreamLink(const QString &aStreamUrl, const QString &aFileName, const QString &aChannel) {
//    QProcess *program = new QProcess;
//    program->start(streamLink_, QStringList() << aStreamUrl << quality_ << aFileName << "--subprocess-errorlog" << "--subprocess-errorlog-path ./logs" << "-o" );
//    program->waitForStarted();
//    qInfo() << "streamLink was started with channel " + aChannel;
//    QObject::connect(program, &QProcess::finished, [=](int lCode, QProcess::ExitStatus lState) {
//        if (lState != QProcess::ExitStatus::NormalExit) {
//            qWarning() << "last error is" << program->error();
//        }
//        qInfo() << "program was finished with channel " + aChannel + " (" + QString::number(lCode) + ")";
//        onlineChannels_.removeOne(aChannel);
//        program->deleteLater();
//    });
////    program.waitForFinished(-1);
////    qWarning() << "last error is" << program.error();
////    program.waitForFinished(-1);
////    qWarning() << "last error2 is" << program.error();
////    program.close();
////    qInfo() << "program was finished with channel " + aChannel;
//}
