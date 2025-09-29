#pragma once

#include <QByteArray>
#include <QDataStream>

enum class MessageType : int
{
    Unknown = 0,
    SendSingleAttractorPoint,
    SendAllAttractorPoints,
    System,
    GetMethod,
    test
};

inline QDataStream &operator<<(QDataStream &out, const MessageType &mt)
{
    qint32 v = static_cast<qint32>(mt);
    out << v;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, MessageType &mt)
{
    qint32 v;
    in >> v;
    if (v < static_cast<qint32>(MessageType::Unknown) || v > static_cast<qint32>(MessageType::test))
    {
        mt = MessageType::Unknown;
    }
    else
    {
        mt = static_cast<MessageType>(v);
    }
    return in;
}
