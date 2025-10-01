#pragma once

#include "ConnectionTypes.hpp"
#include <QObject>
#include <QString>

class FrontendBase : public QObject
{
    Q_OBJECT

  public:
    // Запрещаем копирование
    FrontendBase(const FrontendBase &) = delete;
    FrontendBase &operator=(const FrontendBase &) = delete;

    static FrontendBase *instance();

  public slots:
    void processMessage(MessageType type, QByteArray &data);

  signals:
    void sendMessage(MessageType type, QByteArray &data);

  private:
    explicit FrontendBase(QObject *parent = nullptr);
    ~FrontendBase() override;
    static void cleanupFrontendBase();
};
