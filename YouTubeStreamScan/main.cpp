#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <Windows.h>
#include "cmd.h"

QString filePath_ = "channels.txt";
QString programmSaveVideoPath_ = "streamlink.exe";
QString dirVideoSave_ = "videos\\";
QString quality_ = "best";
Cmd cmd_;

QScopedPointer<QFile> logFile_;

QStringList getChannelList();
QString checkChannel(const QString &channel);
bool mainLoop();
bool createDir();
void initLog();
void log(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QObject::connect(&cmd_, &Cmd::s_closeApp, &a, &QCoreApplication::quit);

    switch (argc) {
    default:
    case 5:
        dirVideoSave_ = argv[4];
        [[fallthrough]];
    case 4:
        quality_ = argv[3];
        [[fallthrough]];
    case 3:
        filePath_ = argv[2];
        [[fallthrough]];
    case 2:
        programmSaveVideoPath_ = argv[1];
        [[fallthrough]];
    case 1:
    case 0: {}
    }

    qInfo() << "programm started with parameters:"
            << "programmSaveVideoPath:" << programmSaveVideoPath_
            << "filePath:" << filePath_
            << "quality:" << quality_
            << "dirVideoSave:" << dirVideoSave_;

    initLog();

    qInfo() << "start scanning";
    mainLoop();
    qInfo() << "scanning is end";

    QTimer::singleShot(10, &cmd_, &Cmd::closeApp);
    return a.exec();
}

bool createDir(const QString &aPath) {
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

bool mainLoop() {
    while (true) {
        QStringList list = getChannelList();
        if (list.count() == 0) {
            return false;
        }
        for (const QString &channel: list) {
            QString streamUrl = checkChannel(channel);
            if (streamUrl != "") {
                qInfo() << "Url is finded:" << streamUrl;
                QString youtubeIndex = streamUrl;
                youtubeIndex = youtubeIndex.remove("https://").remove("www.").remove("youtube.com/").remove("watch?v=") ;
                QString fileName = youtubeIndex;

                createDir(dirVideoSave_);

                int fileIndex = 0;
                while(QFile::exists(dirVideoSave_ + fileName + ".mp4")) {
                    fileName = youtubeIndex + "_" + QString::number(++fileIndex);
                }

                QProcess program;
                program.start(programmSaveVideoPath_, QStringList() << streamUrl << quality_ << "-o" << (dirVideoSave_ + fileName + ".mp4"));
                program.waitForStarted();
                qInfo() << "program was started";
                program.waitForFinished(-1);
                program.close();
                qInfo() << "program was finished";
                //return true;
            }
        }
        Cmd::wait(10000);
    }
    return true;
}

QStringList getChannelList() {
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

QString checkChannel(const QString &aChannel) {
    return cmd_.curl(aChannel);
}

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
    qDebug() << aMessage.toUtf8();
    qInstallMessageHandler(log);
}

void initLog() {
    QString logsPath = "logs\\";
    createDir(logsPath);

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
