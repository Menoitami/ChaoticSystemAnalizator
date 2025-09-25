#pragma once

#include "basewidget.h"
#include <QWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QVector>
#include <QVector3D>          // ← добавлено
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyData.h>

// Forward declarations
class QVTKOpenGLNativeWidget;
class vtkRenderWindow;
class vtkRenderer;
class vtkPolyDataMapper;

namespace Ui {
class PhasePortrait;
}

class PhasePortrait : public BaseWidget
{
    Q_OBJECT

  public:
    explicit PhasePortrait(QWidget *parent = nullptr);
    ~PhasePortrait() override;

    QStringList getTitle() const override;

           // Принимаем QVector<QVector3D>
    void setTrajectory(const QVector<QVector3D>& points);

  private:
    void initializeVTK();
    void updateTrajectoryActor();

    Ui::PhasePortrait *ui;
    bool vtkInitialized = false;

    QVTKOpenGLNativeWidget* vtkWidget = nullptr;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkPolyData> trajectoryData;
    vtkSmartPointer<vtkPolyDataMapper> trajectoryMapper;
    vtkSmartPointer<vtkActor> trajectoryActor;

    QVector<QVector3D> currentPoints; // ← изменено
};
