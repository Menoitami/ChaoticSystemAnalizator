#include "inputs_example.h"
#include "ui_inputs_example.h"

#include <QDoubleValidator>
#include <QSet>

Inputs_example::Inputs_example(QWidget *parent) : QWidget(parent), ui(new Ui::Inputs_example)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_StyledBackground);

    ui->widget->setConstantPrefix("double: ");
    ui->widget->setRestrictInputValidator(std::make_shared<QDoubleValidator>(-999.0, 999.0, 2));
    ui->widget->setLabel("QDoubleValidator(-999.0, 999.0, 2), ");

    // Регулярка для IPv4: 0.0.0.0 — 255.255.255.255
    QRegularExpression regexIP(R"(^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$)");

    ui->widget_3->setLabel("ip format");
    auto validatorIP = std::make_shared<QRegularExpressionValidator>(regexIP, this);
    ui->widget_3->setConstantPrefix("Ip: ");
    ui->widget_3->addErrorValidator("wrong IP format", validatorIP);

    ui->widget_4->setLabel("ip format, show intermidiate as error");
    ui->widget_4->setRestrictInputValidator(validatorIP, true);

    QVector allEdits = {ui->cross1, ui->cross2};
    for (auto *currentEdit : allEdits)
    {
        currentEdit->addCrossValidator("Имя должно быть уникальным",
                                       [currentEdit, allEdits]()
                                       {
                                           QString currentText = currentEdit->text().trimmed();

                                           if (currentText.isEmpty())
                                               return true;

                                           int count = 0;
                                           for (auto *otherEdit : allEdits)
                                           {
                                               if (otherEdit != currentEdit)
                                               {
                                                   QString otherText = otherEdit->text().trimmed();
                                                   if (otherText == currentText)
                                                   {
                                                       count++;
                                                   }
                                               }
                                           }

                                           return count == 0;
                                       });
    }

    ui->widget_2->setPlaceholder("plceholder");
    ui->widget_2->setLabel("disabled field");
}

Inputs_example::~Inputs_example() { delete ui; }
