#pragma once

#include "ConnectionTypes.hpp"
#include "schemechecker.h"
#include "systemsettings.h"
#include <ConnectionTypes.hpp>
#include <QDataStream>
#include <QObject>
#include <QThread>

class SystemManager : public QObject
{

    Q_OBJECT
  public:
    SystemManager(const SystemManager &) = delete;
    SystemManager &operator=(const SystemManager &) = delete;
    ~SystemManager();

    static SystemManager *instance();

    void connectWidgets(std::shared_ptr<BaseWidget> wid);

  private:
    static SystemManager *m_instance;

    SystemManager(QObject *parent = nullptr);
    QThread *m_checkerThread = nullptr;
    SchemeChecker *m_checker = nullptr;

    SystemData sysData;
    SystemData testData;

    SystemSettings *schemeWid = nullptr;

    void connectSystemSettings(std::shared_ptr<SystemSettings> schemeWid_);

  private slots:

    void setSystem(SystemData &);
    void onCheckResult(bool success, const QString &message);

  signals:
    void sendData(MessageType type, QByteArray &data);
    void setMessage(const QString &message);
    void stopLoading();
    void startLoading();

    void requestCheck(const SystemData &data);
};
