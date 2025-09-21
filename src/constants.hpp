#pragma once

#include <QColor>
#include <QString>
#include <vector>

namespace SA
{

enum class CustomWidgets
{
    // Graphics
    ExampleWidget,
    SystemSettings,
    PhasePortrait
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
