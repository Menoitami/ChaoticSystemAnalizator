#pragma once

#include <QColor>
#include <QString>
#include <vector>

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
    std::vector<double> startPos;
    std::vector<double> params;
    double h;
};

} // namespace SA
