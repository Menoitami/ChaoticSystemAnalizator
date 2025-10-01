#include "graphic2dwidget.h"

// VTK includes
#include <QVTKOpenGLNativeWidget.h>
#include <vtkAxis.h>
#include <vtkCallbackCommand.h>
#include <vtkChartXY.h>
#include <vtkContext2D.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPlot.h>
#include <vtkRect.h>
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

    chart = vtkNew<vtkChartXY>();
    view->GetScene()->AddItem(chart);

    chart->GetAxis(vtkAxis::LEFT)->SetVisible(true);
    chart->GetAxis(vtkAxis::BOTTOM)->SetVisible(true);

    table = vtkNew<vtkTable>();
    auto xArray = vtkNew<vtkFloatArray>();
    xArray->SetName("X");
    auto yArray = vtkNew<vtkFloatArray>();
    yArray->SetName("Y");
    table->AddColumn(xArray);
    table->AddColumn(yArray);

    renderWindow->GetInteractor()->Initialize();

    auto doubleClickCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    doubleClickCallback->SetCallback(DoubleClickCallback);
    doubleClickCallback->SetClientData(this);
    renderWindow->GetInteractor()->AddObserver(vtkCommand::LeftButtonDoubleClickEvent, doubleClickCallback);

    vtkInitialized = true;
}

void Graphic2DWidget::setTrajectory(const QVector<QVector2D> &points)
{
    currentPoints = points;
    if (!vtkInitialized || !table || !chart)
        return;

    auto xArray = vtkFloatArray::SafeDownCast(table->GetColumnByName("X"));
    auto yArray = vtkFloatArray::SafeDownCast(table->GetColumnByName("Y"));

    if (!xArray || !yArray)
    {
        qWarning() << "X or Y column not found in table!";
        return;
    }

    // === 1. Сбросить оси в автоматический режим ===
    auto xAxis = chart->GetAxis(vtkAxis::BOTTOM);
    auto yAxis = chart->GetAxis(vtkAxis::LEFT);
    xAxis->SetBehavior(vtkAxis::AUTO);
    yAxis->SetBehavior(vtkAxis::AUTO);

    // === 2. Очистить и заполнить данные ===
    xArray->Reset();
    xArray->ClearLookup();
    yArray->Reset();
    xArray->ClearLookup();
    xArray->SetNumberOfTuples(points.size());
    yArray->SetNumberOfTuples(points.size());

    for (int i = 0; i < points.size(); ++i)
    {
        xArray->SetValue(i, points[i].x());
        yArray->SetValue(i, points[i].y());
    }

    // === 3. Обновить график ===
    if (!plot)
    {
        plot = chart->AddPlot(vtkChart::LINE);
        plot->SetColorF(0, 0, 1.0);
    }

    // ⚠️ ОБЯЗАТЕЛЬНО: перепривязать данные, даже если plot уже существует!
    plot->SetInputData(table, "X", "Y");

    // === 4. Пересчитать границы и отрисовать ===
    table->Modified();
    chart->RecalculateBounds(); // теперь работает, потому что оси в AUTO
    chart->Update();
    renderWindow->Render();
}

void Graphic2DWidget::DoubleClickCallback(vtkObject *, unsigned long eventId, void *clientData, void *)
{
    if (eventId == vtkCommand::LeftButtonDoubleClickEvent)
    {
        static_cast<Graphic2DWidget *>(clientData)->handleDoubleClick();
    }
}

void Graphic2DWidget::handleDoubleClick()
{
    if (!renderWindow || !table || currentPoints.isEmpty())
        return;

    int *eventPos = renderWindow->GetInteractor()->GetEventPosition();
    int x = eventPos[0];
    int y = eventPos[1];

    const int margin = 40;

    bool resetX = (y <= margin); // клик внизу → сброс оси X
    bool resetY = (x <= margin); // клик слева → сброс оси Y

    if (!resetX && !resetY)
        return;

    // Получаем массивы данных
    auto xArray = vtkFloatArray::SafeDownCast(table->GetColumnByName("X"));
    auto yArray = vtkFloatArray::SafeDownCast(table->GetColumnByName("Y"));

    xArray->Modified();
    yArray->Modified();

    if (resetX && xArray && xArray->GetNumberOfTuples() > 0)
    {
        double range[2];
        xArray->GetRange(range);

        auto xAxis = chart->GetAxis(vtkAxis::BOTTOM);
        xAxis->SetMinimum(static_cast<float>(range[0]));
        xAxis->SetMaximum(static_cast<float>(range[1]));
    }

    if (resetY && yArray && yArray->GetNumberOfTuples() > 0)
    {
        double range[2];
        yArray->GetRange(range);

        auto yAxis = chart->GetAxis(vtkAxis::LEFT);
        yAxis->SetMinimum(static_cast<float>(range[0]));
        yAxis->SetMaximum(static_cast<float>(range[1]));
    }

    table->Modified();
    renderWindow->Render();
}
