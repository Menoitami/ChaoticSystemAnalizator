#include "systemsettings.h"
#include "ui_systemsettings.h"
#include <QTimer>

SystemSettings::SystemSettings(QWidget *parent)
    : BaseWidget(SA::CustomWidgets::SystemSettings, parent), ui(new Ui::SystemSettings)
{
    ui->setupUi(this);
    connect(this, &BaseWidget::cancelSig, this, &BaseWidget::destroySig);
    connect(ui->pushButton_check, &QPushButton::clicked, this, &SystemSettings::tryToSetSystem);
    QTimer::singleShot(0, this, [this]() { emit enableSave(true); });
}

SystemSettings::~SystemSettings() { delete ui; }

QStringList SystemSettings::getTitle() const { return {"Настройки", "Разностная схема и параметры"}; }

void SystemSettings::tryToSetSystem()
{

    schemeData.scheme = ui->widget->getScheme();
    schemeData.h = ui->lineEdit_h->text().toDouble();

    schemeData.inits.clear();
    schemeData.params.clear();

    const QStringList poses = ui->lineEdit_startPos->text().split(",");

    schemeData.inits.reserve(poses.length());

    bool ok;
    double value;

    for (const auto &pos : poses)
    {
        value = pos.trimmed().toDouble(&ok);
        if (ok)
        {
            schemeData.inits.push_back(value);
        }
        else
        {
            ui->label_messages->setText("Неправильно введены значения начальной позиции");
            return;
        }
    }

    const QStringList params = ui->lineEdit_params->text().split(",");

    schemeData.params.reserve(params.length());

    for (const auto &param : params)
    {
        value = param.trimmed().toDouble(&ok);
        if (ok)
        {
            schemeData.params.push_back(value);
        }
        else
        {
            ui->label_messages->setText("Неправильно введены значения параметров");
            return;
        }
    }

    emit setSystem(schemeData);
}
