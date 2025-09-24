#pragma once

#include <QByteArray>
#include <QObject>
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
    explicit FrontendBase(const QString& ip = "127.0.0.1", quint16 port = 8080, QObject* parent = nullptr);
    ~FrontendBase() override;

    bool start();
    void stop();
    bool isRunning() const { return running.load(); }
    bool sendMessage(const QByteArray &data);


  signals:
    void connected();
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& err);

  private:
    bool initWinsock();
    void cleanupWinsock();
    bool createAndBind();
    void listenLoop();
    void handleClientSocket(SOCKET clientSock);

    SOCKET server_sock;
    std::atomic<bool> running;
    std::thread listener_thread;
    QString listen_ip;
    quint16 listen_port;

    SOCKET currentClientSock = INVALID_SOCKET;
};
