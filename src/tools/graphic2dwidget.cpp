#include "graphic2dwidget.h"

// VTK includes
#include <QVTKOpenGLNativeWidget.h>
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPlot.h>
#include <vtkTable.h>

#include <QTextStream>
#include <QVector2D>

Graphic2DWidget::Graphic2DWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: white;");

    initializeVTK();
}

Graphic2DWidget::~Graphic2DWidget() = default;

void Graphic2DWidget::initializeVTK()
{
    if (vtkInitialized)
        return;

    vtkWidget = new QVTKOpenGLNativeWidget(this);
    vtkWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(vtkWidget);
    setLayout(layout);

    renderWindow = vtkNew<vtkGenericOpenGLRenderWindow>();
    vtkWidget->setRenderWindow(renderWindow);

    view = vtkNew<vtkContextView>();
    view->SetRenderWindow(renderWindow);

    chart = vtkNew<vtkChartXY>(); // сохраните chart как член класса!
    view->GetScene()->AddItem(chart);

    chart->GetAxis(vtkAxis::LEFT)->SetVisible(true);
    chart->GetAxis(vtkAxis::BOTTOM)->SetVisible(true);
    chart->GetAxis(vtkAxis::LEFT)->SetGridVisible(true);
    chart->GetAxis(vtkAxis::BOTTOM)->SetGridVisible(true);

    table = vtkNew<vtkTable>();
    auto xArray = vtkNew<vtkFloatArray>();
    xArray->SetName("X");
    auto yArray = vtkNew<vtkFloatArray>();
    yArray->SetName("Y");
    table->AddColumn(xArray);
    table->AddColumn(yArray);

    // НЕ добавляем plot здесь!
    plot = nullptr;

    chart->SetShowLegend(false);
    chart->SetAutoAxes(true);

    renderWindow->GetInteractor()->Initialize();
    vtkInitialized = true;
}

void Graphic2DWidget::setTrajectory(const QVector<QVector2D> &points)
{
    currentPoints = points;
    if (!vtkInitialized)
        return;

    auto xArray = vtkFloatArray::SafeDownCast(table->GetColumnByName("X"));
    auto yArray = vtkFloatArray::SafeDownCast(table->GetColumnByName("Y"));

    xArray->SetNumberOfTuples(0);
    yArray->SetNumberOfTuples(0);

    if (points.size() < 2)
    {
        // Удаляем или скрываем существующий график
        if (plot && chart)
        {
            chart->RemovePlotInstance(plot);
            plot = nullptr;
        }
        table->Modified();
        renderWindow->Render();
        return;
    }

    for (const auto &p : points)
    {
        xArray->InsertNextValue(p.x());
        yArray->InsertNextValue(p.y());
    }

    // Создаём график только если его нет
    if (!plot)
    {
        plot = chart->AddPlot(vtkChart::LINE);
        plot->SetInputData(table, "X", "Y");
        plot->SetColorF(0, 0, 1.0); // синий (в диапазоне 0.0–1.0)
    }

    chart->RecalculateBounds();
    table->Modified();
    renderWindow->Render();
}
