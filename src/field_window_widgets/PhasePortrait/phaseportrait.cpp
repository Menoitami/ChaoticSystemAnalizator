#include "phaseportrait.h"
#include "ui_phaseportrait.h"

// VTK includes
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkPolyDataMapper.h>
#include <vtkNamedColors.h>
#include <QVTKOpenGLNativeWidget.h>
#include <QPaintEvent>
#include <vtkProperty.h>

// Qt includes (уже есть через заголовок, но для ясности)
#include <QVector3D>

PhasePortrait::PhasePortrait(QWidget *parent)
    : BaseWidget(SA::CustomWidgets::PhasePortrait, parent), ui(new Ui::PhasePortrait)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: white;");
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    initializeVTK();

    QVector<QVector3D> traj;
    traj << QVector3D(0, 0, 0)
         << QVector3D(1, 1, 1)
         << QVector3D(2, 0, -1)
         << QVector3D(3, -1, 0);

    setTrajectory(traj);
}

PhasePortrait::~PhasePortrait()
{
    delete ui;
}

QStringList PhasePortrait::getTitle() const
{
    return {"Графики", "Фазовый портрет"};
}


void PhasePortrait::initializeVTK()
{
    if (vtkInitialized) return;

    vtkWidget = new QVTKOpenGLNativeWidget(this);
    vtkWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->layout()->addWidget(vtkWidget);

    renderWindow = vtkNew<vtkGenericOpenGLRenderWindow>();
    vtkWidget->setRenderWindow(renderWindow);

    renderer = vtkNew<vtkRenderer>();
    renderer->SetBackground(1.0, 1.0, 1.0);
    renderWindow->AddRenderer(renderer);

           // Инициализация данных и маппера
    trajectoryData = vtkNew<vtkPolyData>();
    trajectoryMapper = vtkNew<vtkPolyDataMapper>();
    trajectoryMapper->SetInputData(trajectoryData);

    trajectoryActor = vtkNew<vtkActor>();
    trajectoryActor->SetMapper(trajectoryMapper);
    trajectoryActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // синий
    trajectoryActor->GetProperty()->SetLineWidth(2.0);

    renderer->AddActor(trajectoryActor);
    renderer->ResetCamera();

    renderWindow->GetInteractor()->Initialize();

    vtkInitialized = true;
}

void PhasePortrait::setTrajectory(const QVector<QVector3D>& points)
{
    currentPoints = points;
    if (vtkInitialized) {
        updateTrajectoryActor();
    }
}

void PhasePortrait::updateTrajectoryActor()
{
    if (!trajectoryData || currentPoints.size() < 2) {
        return;
    }

    auto points = vtkNew<vtkPoints>();
    for (const auto& p : currentPoints) {
        points->InsertNextPoint(p.x(), p.y(), p.z());
    }

    auto polyline = vtkNew<vtkPolyLine>();
    polyline->GetPointIds()->SetNumberOfIds(static_cast<vtkIdType>(currentPoints.size()));
    for (int i = 0; i < currentPoints.size(); ++i) {
        polyline->GetPointIds()->SetId(static_cast<vtkIdType>(i), static_cast<vtkIdType>(i));
    }

    auto cells = vtkNew<vtkCellArray>();
    cells->InsertNextCell(polyline);

    trajectoryData->SetPoints(points);
    trajectoryData->SetLines(cells);
    trajectoryData->Modified();

    renderer->ResetCamera();
    renderWindow->Render();
}
