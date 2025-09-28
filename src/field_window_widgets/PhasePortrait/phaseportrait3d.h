#pragma once

#include "basewidget.h"
#include "graphic3dwidget.h"

#include <QTimer>
#include <QVBoxLayout>
#include <QVector3D>
#include <QVector>
#include <QWidget>

namespace Ui
{
class PhasePortrait;
}

class PhasePortrait3D : public BaseWidget
{
    Q_OBJECT

  public:
    explicit PhasePortrait3D(QWidget *parent = nullptr);
    ~PhasePortrait3D() override;

    QStringList getTitle() const override;
    QVector<QVector3D> loadTrajectoryFromFile(QFile &file);

  private:
    Ui::PhasePortrait *ui;

    Graphic3DWidget *graphicWid;
};
