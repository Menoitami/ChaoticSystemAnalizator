#pragma once

#include "constants.hpp"
#include "systemsettings.h"
#include <ConnectionTypes.hpp>
#include <QDataStream>
#include <QObject>

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
    SystemManager(QObject *parent = nullptr);
    std::pair<bool, QString> checkScheme(SA::SystemData &);

    static SystemManager *m_instance;

    SA::SystemData sysData;

    SystemSettings *schemeWid = nullptr;
    void connectSystemSettings(std::shared_ptr<SystemSettings> schemeWid_);

  private slots:

    void setSystem(SA::SystemData &);
    void sendSystemToBack();

  signals:
    void sendSystemToBack_sig(MessageType type, const QByteArray &data);
};
