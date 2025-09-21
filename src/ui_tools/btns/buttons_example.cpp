#include "buttons_example.h"
#include "ui_buttons_example.h"

ButtonsExample::ButtonsExample(QWidget *parent)
    : QWidget(parent), ui(new Ui::buttons_example)
{
    ui->setupUi(this);
    ui->pushButton_14->setSvgIconPath(":Close_24");
    ui->pushButton_15->setSvgIconPath(":Close_24");
    ui->pushButton_18->setSvgIconPath(":Close_24");
    ui->pushButton_19->setSvgIconPath(":Close_24");
    ui->pushButton_22->setSvgIconPath(":Close_24");
    ui->pushButton_23->setSvgIconPath(":Close_24");

    update();
}

ButtonsExample::~ButtonsExample() { delete ui; }
