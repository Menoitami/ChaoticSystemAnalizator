#include "phaseportrait3d.h"
#include "ui_phaseportrait3d.h"

PhasePortrait3D::PhasePortrait3D(QWidget *parent)
    : BaseWidget(SA::CustomWidgets::PhasePortrait3D, parent), ui(new Ui::PhasePortrait)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: white;");

    graphicWid = ui->widget;

    QVector<QVector3D> traj;
    QFile file = QFile("E:/progs/ChaoticSystemAnalizator/AnalyticsBackend/tmp/output/Attractor.csv");
    traj = loadTrajectoryFromFile(file);
    graphicWid->setTrajectory(traj);
}

PhasePortrait3D::~PhasePortrait3D() { delete ui; }

QStringList PhasePortrait3D::getTitle() const { return {"Графики", "Фазовый портрет"}; }

QVector<QVector3D> PhasePortrait3D::loadTrajectoryFromFile(QFile &file)
{

    QVector<QVector3D> trajectory;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Не удалось открыть файл:" << file.fileName();
        return trajectory;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList parts = line.split(',', Qt::SkipEmptyParts);
        if (parts.size() < 3)
        {
            qWarning() << "Некорректная строка:" << line;
            continue;
        }

        bool ok1, ok2, ok3;
        float x = parts[0].toFloat(&ok1);
        float y = parts[1].toFloat(&ok2);
        float z = parts[2].toFloat(&ok3);

        if (ok1 && ok2 && ok3)
        {
            trajectory.append(QVector3D(x, y, z));
        }
        else
        {
            qWarning() << "Ошибка преобразования чисел в строке:" << line;
        }
    }

    file.close();
    return trajectory;
}
