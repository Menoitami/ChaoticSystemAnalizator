#include "BackendBase.hpp"
#include <QCoreApplication>
#include <QDebug>

void BackendBase::cleanupBackendBase() { delete BackendBase::instance(); }

BackendBase *BackendBase::instance()
{
    static BackendBase *self = nullptr;
    if (!self)
    {
        self = new BackendBase(qApp);
        qAddPostRoutine(cleanupBackendBase);
    }
    return self;
}

BackendBase::BackendBase(QObject *parent) : QObject(parent) {}

BackendBase::~BackendBase() {}

void BackendBase::processMessage(MessageType type, const QByteArray &data)
{
    Q_UNUSED(type);
    Q_UNUSED(data);

    qDebug() << "я че то получил";
}
