#pragma once

#include <QTimer>
#include <QVBoxLayout>
#include <QVector3D> // ← добавлено
#include <QVector>
#include <QWidget>
#include <vtkActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>

#include <QFile>

// Forward declarations
class QVTKOpenGLNativeWidget;
class vtkRenderWindow;
class vtkRenderer;
class vtkPolyDataMapper;

class Graphic3DWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit Graphic3DWidget(QWidget *parent = nullptr);
    virtual ~Graphic3DWidget() override;

    void setTrajectory(const QVector<QVector3D> &points);

  public slots:

    void rotateCamera(float degree = 0.2);

  private:
    void initializeVTK();
    void updateTrajectoryActor();

    QTimer *rotationTimer = nullptr;
    bool vtkInitialized = false;

    QVTKOpenGLNativeWidget *vtkWidget = nullptr;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkPolyData> trajectoryData;
    vtkSmartPointer<vtkPolyDataMapper> trajectoryMapper;
    vtkSmartPointer<vtkActor> trajectoryActor;
    vtkSmartPointer<vtkCubeAxesActor> axes;

    vtkSmartPointer<vtkTextActor> shortAxisLabelX;
    vtkSmartPointer<vtkTextActor> shortAxisLabelY;
    vtkSmartPointer<vtkTextActor> shortAxisLabelZ;
    bool hasShortAxisLabel = false;

    QVector<QVector3D> currentPoints;
};
