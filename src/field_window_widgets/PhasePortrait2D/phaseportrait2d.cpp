#include "phaseportrait2d.h"
#include "ui_phaseportrait2d.h"
#include <QRadioButton>

PhasePortrait2D::PhasePortrait2D(QWidget *parent)
    : BaseWidget(SA::CustomWidgets::PhasePortrait2D, parent), ui(new Ui::PhasePortrait2D)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: white;");

    graphicWid = ui->graphicWidget;

    QVector<QVector<double>> traj;
    QFile file = QFile("E:/progs/ChaoticSystemAnalizator/AnalyticsBackend/tmp/output/Attractor.csv");
    traj = loadTrajectoryFromFile(file);
    // graphicWid->setTrajectory(traj);

    ui->scrollAreaWidgetContents_horizontal->layout()->setAlignment(Qt::AlignCenter);
    ui->scrollAreaWidgetContents_vertical->layout()->setAlignment(Qt::AlignCenter);

    setData(traj);
}

PhasePortrait2D::~PhasePortrait2D() { delete ui; }

QStringList PhasePortrait2D::getTitle() const { return {"Графики", "Фазовый портрет 2D"}; }

QVector<QVector<double>> PhasePortrait2D::loadTrajectoryFromFile(QFile &file)
{
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file:" << file.fileName();
        return {};
    }

    QTextStream in(&file);
    QVector<QVector<double>> trajectory;
    int expectedSize = -1;

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList parts = line.split(',', Qt::SkipEmptyParts);
        QVector<double> row;
        row.reserve(parts.size());

        for (const QString &part : parts)
        {
            bool ok = false;
            double value = part.trimmed().toDouble(&ok);
            if (!ok)
            {
                qCritical() << "Failed to parse number in line:" << line << "token:" << part;
                file.close();
                return {};
            }
            row.append(value);
        }

        if (expectedSize == -1)
        {
            if (row.isEmpty())
            {
                qCritical() << "First non-empty line contains no valid numbers";
                file.close();
                return {};
            }
            expectedSize = row.size();
        }
        else if (row.size() != expectedSize)
        {
            qCritical() << "Inconsistent number of values in line:" << line << "Expected:" << expectedSize
                        << "Got:" << row.size();
            file.close();
            return {};
        }

        trajectory.append(row);
    }

    file.close();

    if (trajectory.isEmpty())
    {
        qWarning() << "File is empty or contains only empty lines";
        return {};
    }

    return trajectory;
}

void PhasePortrait2D::setData(QVector<QVector<double>> &data_)
{
    if (!data_.count())
        return;

    if (!data_[0].count())
        return;

    coordsCount = data_[0].count();
    data = data_;

    setScrollAreas();
}

void PhasePortrait2D::setScrollAreas()
{
    if (coordsCount == 0)
        return;

    // Очистка предыдущего содержимого (на случай повторного вызова)
    clearScrollArea(ui->scrollAreaWidgetContents_vertical);
    clearScrollArea(ui->scrollAreaWidgetContents_horizontal);

    // Создание layout'ов, если они ещё не созданы
    auto setupLayout = [](QWidget *container)
    {
        if (container->layout() == nullptr)
        {
            auto layout = new QVBoxLayout(container);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(5);
            container->setLayout(layout);
        }
    };

    setupLayout(ui->scrollAreaWidgetContents_vertical);
    setupLayout(ui->scrollAreaWidgetContents_horizontal);

    // Добавление радиокнопок
    for (int i = 0; i < coordsCount; ++i)
    {
        QString label = QString("X%1").arg(i + 1);

        // Вертикальная ось
        QRadioButton *rbVert = new QRadioButton(label, ui->scrollAreaWidgetContents_vertical);
        rbVert->setProperty("index", i + 1);
        rbVert->setProperty("axis", "vertical");
        connect(rbVert, &QRadioButton::toggled, this, &PhasePortrait2D::onRadioButtonToggled);
        ui->scrollAreaWidgetContents_vertical->layout()->addWidget(rbVert);

        // Горизонтальная ось
        QRadioButton *rbHoriz = new QRadioButton(label, ui->scrollAreaWidgetContents_horizontal);
        rbHoriz->setProperty("index", i + 1);
        rbHoriz->setProperty("axis", "horizontal");
        connect(rbHoriz, &QRadioButton::toggled, this, &PhasePortrait2D::onRadioButtonToggled);
        ui->scrollAreaWidgetContents_horizontal->layout()->addWidget(rbHoriz);

        if (i == 0)
        {

            rbVert->setChecked(true);
            rbHoriz->setChecked(true);
        }
    }
}

void PhasePortrait2D::clearScrollArea(QWidget *container)
{
    if (!container || !container->layout())
        return;

    QLayout *layout = container->layout();
    while (QLayoutItem *item = layout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }
}

void PhasePortrait2D::onRadioButtonToggled(bool checked)
{
    if (!checked)
        return;

    QRadioButton *rb = qobject_cast<QRadioButton *>(sender());
    if (!rb)
        return;

    int index = rb->property("index").toInt();
    QString axis = rb->property("axis").toString();

    if (axis == "horizontal")
        selectedXIndex = index - 1;
    else if (axis == "vertical")
        selectedYIndex = index - 1;

    if (selectedXIndex >= 0 && selectedYIndex >= 0)
    {
        if (data.isEmpty())
            return;

        size_t numRows = data.size();
        size_t numCols = data[0].size();

        if (selectedXIndex >= static_cast<int>(numCols) || selectedYIndex >= static_cast<int>(numCols))
        {
            qWarning() << "Selected column index out of range";
            return;
        }

        shownData.clear();
        shownData.reserve(numRows);

        if (selectedXIndex == selectedYIndex)
        {
            double x = 0;
            for (const auto &row : data)
            {
                double y = row[selectedYIndex];
                x += h;
                shownData.append(QVector2D(static_cast<double>(x), static_cast<double>(y)));
            }
        }
        else
        {
            for (const auto &row : data)
            {
                double x = row[selectedXIndex];
                double y = row[selectedYIndex];
                shownData.append(QVector2D(static_cast<double>(x), static_cast<double>(y)));
            }
        }

        if (graphicWid)
            graphicWid->setTrajectory(shownData);
    }
}
