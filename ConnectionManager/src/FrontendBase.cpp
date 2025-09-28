#include "FrontendBase.hpp"
#include "ConnectionTypes.hpp"

#include <QMetaObject>
#include <QCoreApplication>

FrontendBase::FrontendBase(const QString &ip, quint16 port, QObject *parent)
    : ConnectionUnit(ip,port,parent)
{
}

FrontendBase::~FrontendBase() {
}



void FrontendBase::send_custom_messge(QString str)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);

    out << MessageType::test;
    out << str;
    sendMessage(buffer);
}

void FrontendBase::processMessage(const QByteArray &data, const QHostAddress &from, quint16 port)
{
    QDataStream in(data);

    MessageType mt;
    in >> mt;

    switch (mt){
    case MessageType::Unknown:
    case MessageType::SendSingleAttractorPoint:
    case MessageType::SendAllAttractorPoints:
    case MessageType::GetSystem:
    case MessageType::GetMethod:
    case MessageType::test:
        QString text;
        in >> text;
        qDebug() << text;
        break;
    }
}
