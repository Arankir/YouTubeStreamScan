#ifndef CMD_H
#define CMD_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QEventLoop>
#include <chrono>
#include <thread>
#include <QTimer>

class Cmd : public QObject {
    Q_OBJECT
public:
    explicit Cmd(QObject *parent = nullptr);
    ~Cmd();
    QString curl(const QString &channel);
    QString command(const QString &comand);
    static void wait(int mSeconds);

public slots:
    void closeApp();

signals:
    void s_closeApp();

private:
    QProcess process_;

};

#endif // CMD_H
