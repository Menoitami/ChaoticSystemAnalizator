#pragma once

#include <QByteArray>
#include <QObject>
#include <QHostAddress>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <atomic>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class FrontendBase : public QObject {
    Q_OBJECT
  public:
    explicit FrontendBase(const QString &ip, quint16 port, QObject *parent = nullptr);
    ~FrontendBase();

    bool start();
    void stop();
    bool isRunning() const { return running.load(); }

  signals:
    void dataReceived(const QByteArray &data, const QHostAddress &from, quint16 port);
    void errorOccurred(const QString &err);

  private:
    bool initWinsock();
    void cleanupWinsock();
    bool createAndBind();
    void recvLoop();

  private:
    QString listen_ip;
    quint16 listen_port;

    std::atomic<bool> running;
    std::thread worker_thread;

    SOCKET sock; // UDP socket
};
