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

class ConnectionUnit : public QObject {
    Q_OBJECT
  public:
    explicit ConnectionUnit(const QString &ip="127.0.0.1", quint16 port = 8080, QObject *parent = nullptr);
    ~ConnectionUnit();
    bool isRunning() const { return running.load(); }
    bool sendMessage(const QByteArray &data, const QHostAddress &addr =QHostAddress("127.0.0.1") , quint16 port= 8080);

  signals:
    void dataReceived(const QByteArray &data, const QHostAddress &from, quint16 port);
    void errorOccurred(const QString &err);

  private:
    bool start();
    void stop();
    bool initWinsock();
    void cleanupWinsock();
    bool createAndBind();
    void recvLoop();
    virtual void processMessage(const QByteArray &data, const QHostAddress &from, quint16 port) = 0;

  private:
    QString listen_ip;
    quint16 listen_port;

    std::atomic<bool> running;
    std::thread worker_thread;

    SOCKET sock; // UDP socket
};
