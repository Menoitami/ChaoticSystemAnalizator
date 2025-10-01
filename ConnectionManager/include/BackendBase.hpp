#pragma once

#include "ConnectionTypes.hpp"
#include <QObject>
#include <QString>

class BackendBase : public QObject
{
    Q_OBJECT

  public:
    BackendBase(const BackendBase &) = delete;
    BackendBase &operator=(const BackendBase &) = delete;

    static BackendBase *instance();

  public slots:
    void processMessage(MessageType type, QByteArray &data);

  signals:
    void sendMessage(MessageType type, QByteArray &data);

    void handle_get_system(QString sys);

  private:
    explicit BackendBase(QObject *parent = nullptr);
    ~BackendBase() override;

    static void cleanupBackendBase();
};
