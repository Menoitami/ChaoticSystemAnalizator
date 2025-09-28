#include "graphic3dwidget.h"

// VTK includes
#include <QPaintEvent>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

#include <QVector3D>

Graphic3DWidget::Graphic3DWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: white;");

    initializeVTK();
}

Graphic3DWidget::~Graphic3DWidget() {}

void Graphic3DWidget::initializeVTK()
{
    if (vtkInitialized)
        return;

    vtkWidget = new QVTKOpenGLNativeWidget(this);
    vtkWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(vtkWidget);
    setLayout(layout);

    renderWindow = vtkNew<vtkGenericOpenGLRenderWindow>();
    vtkWidget->setRenderWindow(renderWindow);

    renderer = vtkNew<vtkRenderer>();
    renderer->SetBackground(1.0, 1.0, 1.0);
    renderWindow->AddRenderer(renderer);

    // Траектория
    trajectoryData = vtkNew<vtkPolyData>();
    trajectoryMapper = vtkNew<vtkPolyDataMapper>();
    trajectoryMapper->SetInputData(trajectoryData);

    trajectoryActor = vtkNew<vtkActor>();
    trajectoryActor->SetMapper(trajectoryMapper);
    trajectoryActor->GetProperty()->SetLineWidth(2.0);
    trajectoryActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    renderer->AddActor(trajectoryActor);

    // === Добавляем координатные оси с сеткой ===
    axes = vtkNew<vtkCubeAxesActor>();
    axes->SetUse2DMode(0);
    axes->SetFlyModeToFurthestTriad();
    axes->SetBounds(0, 1, 0, 1, 0, 1); // временные границы — обновим позже
    axes->SetCamera(renderer->GetActiveCamera());

    // Настройка внешнего вида
    axes->GetXAxesLinesProperty()->SetColor(0, 0, 0);
    axes->GetYAxesLinesProperty()->SetColor(0, 0, 0);
    axes->GetZAxesLinesProperty()->SetColor(0, 0, 0);

    axes->GetXAxesGridlinesProperty()->SetColor(0, 0, 0);
    axes->GetYAxesGridlinesProperty()->SetColor(0, 0, 0);
    axes->GetZAxesGridlinesProperty()->SetColor(0, 0, 0);

    // Включаем сетку на плоскостях
    axes->SetXAxisVisibility(1);
    axes->SetYAxisVisibility(1);
    axes->SetZAxisVisibility(1);
    axes->SetDrawXGridlines(false);
    axes->SetDrawYGridlines(false);
    axes->SetDrawZGridlines(false);

    // Подписи осей
    axes->SetXTitle("X");
    axes->SetYTitle("Y");
    axes->SetZTitle("Z");

    // Стиль меток
    axes->GetTitleTextProperty(0)->SetColor(0, 0, 0);
    axes->GetLabelTextProperty(0)->SetColor(0, 0, 0);
    axes->GetTitleTextProperty(1)->SetColor(0, 0, 0);
    axes->GetLabelTextProperty(1)->SetColor(0, 0, 0);
    axes->GetTitleTextProperty(2)->SetColor(0, 0, 0);
    axes->GetLabelTextProperty(2)->SetColor(0, 0, 0);

    renderer->AddActor(axes);

    renderer->ResetCamera();

    renderWindow->GetInteractor()->Initialize();
    vtkInitialized = true;

    // rotationTimer = new QTimer(this);
    // connect(rotationTimer, &QTimer::timeout, this, [this]() { rotateCamera(1.0f); });
    // rotationTimer->start(10);
}

void Graphic3DWidget::setTrajectory(const QVector<QVector3D> &points)
{
    currentPoints = points;
    if (vtkInitialized)
    {
        updateTrajectoryActor();
    }
}

void Graphic3DWidget::updateTrajectoryActor()
{
    if (!trajectoryData || currentPoints.size() < 2)
        return;

    // Создаём геометрию траектории
    auto points = vtkNew<vtkPoints>();
    for (const auto &p : currentPoints)
    {
        points->InsertNextPoint(p.x(), p.y(), p.z());
    }

    auto polyline = vtkNew<vtkPolyLine>();
    polyline->GetPointIds()->SetNumberOfIds(static_cast<vtkIdType>(currentPoints.size()));
    for (int i = 0; i < currentPoints.size(); ++i)
    {
        polyline->GetPointIds()->SetId(i, i);
    }

    auto cells = vtkNew<vtkCellArray>();
    cells->InsertNextCell(polyline);

    trajectoryData->SetPoints(points);
    trajectoryData->SetLines(cells);
    trajectoryData->Modified();

    // Получаем реальные границы данных
    double bounds[6];
    trajectoryData->GetBounds(bounds);

    // Центры по осям
    double cx = (bounds[0] + bounds[1]) * 0.5;
    double cy = (bounds[2] + bounds[3]) * 0.5;
    double cz = (bounds[4] + bounds[5]) * 0.5;

    // Максимальный полу-диапазон
    double halfX = (bounds[1] - bounds[0]) * 0.5;
    double halfY = (bounds[3] - bounds[2]) * 0.5;
    double halfZ = (bounds[5] - bounds[4]) * 0.5;
    double maxHalfRange = std::max({halfX, halfY, halfZ});

    // Если все точки совпадают — избегаем нулевого диапазона
    if (maxHalfRange == 0.0)
        maxHalfRange = 1.0;

    // Новые границы с одинаковым масштабом по всем осям
    double uniformBounds[6] = {cx - maxHalfRange, cx + maxHalfRange, cy - maxHalfRange,
                               cy + maxHalfRange, cz - maxHalfRange, cz + maxHalfRange};

    // Передаём в CubeAxesActor
    axes->SetBounds(uniformBounds);
    axes->SetCamera(renderer->GetActiveCamera());

    // Сбрасываем камеру под новый масштаб
    renderer->ResetCamera();
    renderer->ResetCameraClippingRange();
    renderWindow->Render();
}
void Graphic3DWidget::rotateCamera(float degree)
{
    if (!renderer || !renderWindow)
        return;

    auto camera = renderer->GetActiveCamera();
    camera->Azimuth(degree);

    renderer->ResetCameraClippingRange();

    renderWindow->Render();
}
