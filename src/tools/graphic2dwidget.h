#pragma once

#include <QFile>
#include <QVBoxLayout>
#include <QVTKOpenGLNativeWidget.h>
#include <QVector2D>
#include <QVector>
#include <QWidget>
#include <vtkChartXY.h>
#include <vtkSmartPointer.h>

// Forward declarations
class vtkGenericOpenGLRenderWindow;
class vtkContextView;
class vtkTable;
class vtkPlot;

class Graphic2DWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit Graphic2DWidget(QWidget *parent = nullptr);
    ~Graphic2DWidget() override;

    void setTrajectory(const QVector<QVector2D> &points);

  private:
    void initializeVTK();

    QVTKOpenGLNativeWidget *vtkWidget = nullptr;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkContextView> view;
    vtkSmartPointer<vtkTable> table;
    vtkSmartPointer<vtkPlot> plot;
    vtkSmartPointer<vtkChartXY> chart;

    QVector<QVector2D> currentPoints;
    bool vtkInitialized = false;
    void handleDoubleClick();

    static void DoubleClickCallback(vtkObject *, unsigned long eventId, void *clientData, void *);
};
