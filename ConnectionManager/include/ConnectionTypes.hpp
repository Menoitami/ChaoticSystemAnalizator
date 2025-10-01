#pragma once

#include <QByteArray>
#include <QDataStream>
#include <QList>
#include <QString>

enum class MessageType : int
{
    Unknown = 0,
    SendSingleAttractorPoint,
    SendAllAttractorPoints,
    SendSheme,
    SendParams,
    SendInits,
    SendH,
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

struct SystemData
{
    QString scheme;
    QList<double> inits;
    QList<double> params;
    double h;

    friend inline QDataStream &operator<<(QDataStream &out, const SystemData &mt)
    {
        out << mt.scheme;
        out << mt.inits;
        out << mt.params;
        out << mt.h;
        return out;
    }

    friend inline QDataStream &operator>>(QDataStream &in, SystemData &mt)
    {
        in >> mt.scheme;
        in >> mt.inits;
        in >> mt.params;
        in >> mt.h;
        return in;
    }
};
