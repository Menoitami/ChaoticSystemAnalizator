#pragma once

#include "ConnectionUnit.hpp"
#include <QByteArray>
#include <QObject>
#include <QHostAddress>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class FrontendBase : public ConnectionUnit {
    Q_OBJECT
  public:
    explicit FrontendBase(const QString &ip = "127.0.0.1", quint16 port = 8080, QObject *parent = nullptr);
    ~FrontendBase();

    void send_custom_messge(QString str);

  private:

    void processMessage(const QByteArray &data, const QHostAddress &from, quint16 port) override;

  private:

};
