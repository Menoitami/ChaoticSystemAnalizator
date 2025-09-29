#pragma once

#include <QColor>
#include <QDataStream>
#include <QList>
#include <QString>

namespace SA
{

enum class CustomWidgets
{
    ExampleWidget,
    SystemSettings,
    PhasePortrait3D,
    PhasePortrait2D
};

enum class Sections
{
    Project,
    Graphics,
};

struct SystemData
{
    QString scheme;
    QList<double> startPos;
    QList<double> params;
    double h;

    friend inline QDataStream &operator<<(QDataStream &out, const SystemData &mt)
    {
        out << mt.scheme;
        out << mt.startPos;
        out << mt.params;
        out << mt.h;
        return out;
    }

    friend inline QDataStream &operator>>(QDataStream &in, SystemData &mt)
    {
        in >> mt.scheme;
        in >> mt.startPos;
        in >> mt.params;
        in >> mt.h;
        return in;
    }
};

} // namespace SA
