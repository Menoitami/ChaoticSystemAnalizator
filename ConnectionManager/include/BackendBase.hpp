#pragma once
#include "ConnectionUnit.hpp"
#include <QObject>
#include <QString>
#include <winsock2.h>
#include <ws2tcpip.h>

class BackendBase : public ConnectionUnit {
    Q_OBJECT
  public:
    explicit BackendBase(const QString &ip = "127.0.0.1", quint16 port = 8080, QObject *parent = nullptr);
    ~BackendBase();

    void send_custom_messge(QString str);

  private:

    void processMessage(const QByteArray &data, const QHostAddress &from, quint16 port) override;

  private:

};
