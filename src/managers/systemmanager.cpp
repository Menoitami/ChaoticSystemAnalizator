#include "SystemManager.h"
#include "QDebug"
#include "config.h"
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>

SystemManager *SystemManager::m_instance = nullptr;

SystemManager::SystemManager(QObject *parent) : QObject(parent)
{

    m_checkerThread = new QThread(this);
    m_checker = new SchemeChecker();
    m_checker->moveToThread(m_checkerThread);

    connect(m_checkerThread, &QThread::finished, m_checker, &QObject::deleteLater);
    connect(this, &SystemManager::requestCheck, m_checker, &SchemeChecker::check);
    connect(m_checker, &SchemeChecker::checked, this, &SystemManager::onCheckResult);

    m_checkerThread->start();
}

SystemManager::~SystemManager()
{
    qDebug() << __FUNCTION__;
    if (m_checkerThread)
    {
        m_checkerThread->quit();     // Послать сигнал завершения event loop'а
        m_checkerThread->wait(3000); // Ждать до 3 секунд
        // Если не завершился — можно принудительно, но лучше избегать
        if (m_checkerThread->isRunning())
        {
            qWarning() << "SchemeChecker thread did not finish in time!";
            // m_checkerThread->terminate(); // ← НЕ рекомендуется!
        }
        delete m_checkerThread; // Теперь можно удалить
    }
}

SystemManager *SystemManager::instance()
{
    if (!m_instance)
    {
        m_instance = new SystemManager(qApp);
    }
    return m_instance;
}

void SystemManager::setSystem(SystemData &data)
{
    emit startLoading();
    emit setMessage("Validating scheme...");
    testData = data;

    emit requestCheck(data);
}

void SystemManager::connectWidgets(std::shared_ptr<BaseWidget> wid)
{

    switch (wid->getName())
    {
    case (SA::CustomWidgets::SystemSettings):
        connectSystemSettings(std::dynamic_pointer_cast<SystemSettings>(wid));
        break;

    default:
        break;
    };
}

void SystemManager::connectSystemSettings(std::shared_ptr<SystemSettings> schemeWid_)
{
    if (schemeWid_ == nullptr)
        return;

    schemeWid = schemeWid_.get();

    connect(schemeWid, &SystemSettings::setSystem, this, &SystemManager::setSystem);
    connect(schemeWid, &QWidget::destroyed, this, [this]() { schemeWid = nullptr; });
    connect(this, &SystemManager::setMessage, schemeWid, &SystemSettings::setMessage);
    connect(this, &SystemManager::stopLoading, schemeWid, &SystemSettings::stopLoading);
    connect(this, &SystemManager::startLoading, schemeWid, &SystemSettings::startLoading);
}

void SystemManager::onCheckResult(bool success, const QString &message)
{
    emit setMessage(message);
    emit stopLoading();
    if (!success)
    {
        return;
    }

    if (sysData.h != testData.h)
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream << testData.h;
        emit sendData(MessageType::SendH, buffer);
    }

    if (sysData.params != testData.params)
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream << testData.params;
        emit sendData(MessageType::SendParams, buffer);
    }

    if (sysData.inits != testData.inits)
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream << testData.inits;
        emit sendData(MessageType::SendInits, buffer);
    }

    if (sysData.scheme != testData.scheme)
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream << testData.scheme;
        emit sendData(MessageType::SendSheme, buffer);
    }

    sysData = testData;
}
