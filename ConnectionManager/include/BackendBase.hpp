#pragma once
#include <QObject>
#include <QString>
#include <thread>
#include <atomic>
#include <winsock2.h>
#include <ws2tcpip.h>

class BackendBase : public QObject {
    Q_OBJECT
  public:
    explicit BackendBase(const QString &destIp, quint16 destPort, QObject *parent = nullptr);
    ~BackendBase();

    bool start();   // инициализация и отправка стартового сообщения
    void stop();

  signals:
    void errorOccurred(const QString &msg);

  private:
    bool initWinsock();
    void cleanupWinsock();
    bool createSocket();
    bool sendOnce(const QByteArray &data);

    QString dest_ip;
    quint16 dest_port;
    std::atomic<bool> running;
    SOCKET sock;
    sockaddr_in destAddr;
    std::thread worker_thread;
};
