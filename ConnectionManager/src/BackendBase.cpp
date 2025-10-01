#include "BackendBase.hpp"
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QIODevice>

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

void BackendBase::processMessage(MessageType type, QByteArray &data)
{

    switch (type)
    {
    case MessageType::SendSheme:
    {
        QString scheme;
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream >> scheme;
        qDebug() << scheme;
        break;
    }
    case MessageType::SendParams:
    {
        QList<double> params;
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream >> params;
        qDebug() << params;
        break;
    }
    case MessageType::SendInits:
    {
        QList<double> inits;
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream >> inits;
        qDebug() << inits;
        break;
    }
    case MessageType::SendH:
    {
        double h;
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream >> h;
        qDebug() << h;
        break;
    }

    default:
        break;
    }
}
