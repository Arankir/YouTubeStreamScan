#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <Windows.h>
#include "mainwork.h"
#include "cmd.h"

QString filePath_ = "channels.txt";
QString streamLink_ = "streamlink.exe";
QString dirVideoSave_ = "videos/";
QString quality_ = "best";
Cmd cmd_;

QStringList onlineChannels_;

QScopedPointer<QFile> logFile_;

//QStringList getChannelList();
//QString checkChannel(const QString &channel);
//bool mainLoop();
//bool createDir();
QString getVideoFileName(QString url);
void openStreamLink(const QString &streamUrl, const QString &fileName, const QString &aChannel);
void initLog();
void log(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QObject::connect(&cmd_, &Cmd::s_closeApp, &a, &QCoreApplication::quit);

    switch (argc) {
    default:
    case 4:
        dirVideoSave_ = argv[4];
        [[fallthrough]];
    case 3:
        quality_ = argv[3];
        [[fallthrough]];
    case 2:
        filePath_ = argv[2];
        [[fallthrough]];
    case 1:
    case 0: {}
    }

    initLog();

    qInfo() << "programm started with parameters:"
            << "filePath:" << filePath_
            << "quality:" << quality_
            << "dirVideoSave:" << dirVideoSave_;

    //qInfo() << "start scanning";
    MainWork work(filePath_, streamLink_, dirVideoSave_, quality_);
    //mainLoop();
    //qInfo() << "scanning is end";

    QTimer::singleShot(10, &cmd_, &Cmd::closeApp);
    return 1;
}

//bool createDir(const QString &aPath) {
//    bool exist = true;
//    QString path = aPath;
//    path.replace("\\", "/");
//    QStringList dirs = path.split("/");
//    if (dirs.last() != "") {
//        dirs.removeLast();
//    }
//    QString pathNow = "";
//    for (auto &dir: dirs) {
//        pathNow += std::move(dir) + "/";
//        exist = (QDir().mkdir(pathNow) && exist);
//    }
//    return exist;
//}

//bool mainLoop() {
//    while (true) {
//        QStringList list = getChannelList();
//        if (list.count() == 0) {
//            return false;
//        }
//        for (const QString &channel: list) {
//            QString streamUrl = checkChannel(channel);
//            if (streamUrl != "") {
//                qInfo() << "url is finded:" << streamUrl;
//                onlineChannels_.append(cmd_.lastChannel());
//                openStreamLink(streamUrl, getVideoFileName(streamUrl), cmd_.lastChannel());
//            }
//        }
//        Cmd::wait(10000);
//    }
//    return true;
//}

//QString getVideoFileName(QString aUrl) {
//    QString fileName = aUrl.remove("https://").remove("www.").remove("youtube.com/").remove("watch?v=");
//    createDir(dirVideoSave_);
//    int fileIndex = 0;

//    while(QFile::exists(dirVideoSave_ + fileName + ".mp4")) {
//        fileName = aUrl + "_" + QString::number(++fileIndex);
//    }
//    qInfo() << "video path:" << dirVideoSave_ + fileName + ".mp4";
//    return dirVideoSave_ + fileName + ".mp4";
//}

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

//QStringList getChannelList() {
//    QFile file(filePath_);
//    if (!file.open(QIODevice::ReadOnly)) {
//        qWarning() << "file with channels is not available";
//        return QStringList();
//    }
//    QStringList list;
//    while (!file.atEnd()) {
//        list << file.readLine();
//    }
//    file.close();
//    return list;
//}

//QString checkChannel(const QString &aChannel) {
//    return cmd_.curl(aChannel);
//}

void log(QtMsgType aType, const QMessageLogContext &aContext, const QString &aMessage) {
    const char *function = aContext.function ? aContext.function : "";
    QTextStream out(logFile_.data());

    switch (aType) {
    case QtInfoMsg:     out << "INFO "; break;
    case QtDebugMsg:    out << "DEBG "; break;
    case QtWarningMsg:  out << "WRNG "; break;
    case QtCriticalMsg: out << "CRCL "; break;
    case QtFatalMsg:    out << "FATL "; break;
    }

    out << QTime::currentTime().toString("hh:mm:ss ");//"yyyy-MM-dd hh:mm:ss "
    out << function << aMessage << endl;
    out.flush();

    qInstallMessageHandler(0);
    if (aType != QtWarningMsg) {
        qDebug() << aMessage.toUtf8();
    }
    qInstallMessageHandler(log);
}

void initLog() {
    QString logsPath = "logs\\";
    MainWork::createDir(logsPath);

    //Удаление старых файлов
    QDir dirLogs(logsPath);
    dirLogs.setFilter(QDir::Files | QDir::NoSymLinks);
    dirLogs.setSorting(QDir::Name);
    QFileInfoList list = dirLogs.entryInfoList();
    for(auto &file: list) {
        if (file.fileName().indexOf("log_") == 0) {
            QDateTime date {QDateTime::fromString(file.fileName().remove("log_").remove(".txt"), "yyyy.MM.dd")};
            if (date < QDateTime::currentDateTime().addMonths(-1)) {
                QFile::remove(file.filePath() + "/" + file.fileName());
            }
        }
    }

    logFile_.reset(new QFile(logsPath + QDateTime::currentDateTime().toString("log_yyyy.MM.dd") + ".txt"));
    logFile_.data()->open(QFile::Append | QFile::Text);
    qInstallMessageHandler(log);
}
