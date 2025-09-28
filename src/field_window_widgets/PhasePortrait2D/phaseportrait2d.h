#pragma once

#include "basewidget.h"
#include "graphic2dwidget.h"

#include <QWidget>

namespace Ui
{
class PhasePortrait2D;
}

class PhasePortrait2D : public BaseWidget
{
    Q_OBJECT

  public:
    explicit PhasePortrait2D(QWidget *parent = nullptr);
    ~PhasePortrait2D();

    QStringList getTitle() const override;
    QVector<QVector<double>> loadTrajectoryFromFile(QFile &file);

    void setData(QVector<QVector<double>> &data);

  private:
    Ui::PhasePortrait2D *ui;

    QVector<QVector<double>> data;
    QVector<QVector2D> shownData;

    quint16 coordsCount;
    int selectedXIndex = -1;
    int selectedYIndex = -1;
    double h = 0.01;

    Graphic2DWidget *graphicWid;

    void setScrollAreas();
    void clearScrollArea(QWidget *container);

  private slots:

    void onRadioButtonToggled(bool checked);
};
