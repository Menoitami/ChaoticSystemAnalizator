#include "examplewidget.h"
#include "ui_examplewidget.h"
#include <QTimer>

ExampleWidget::ExampleWidget(QWidget *parent)
    : BaseWidget(SA::CustomWidgets::ExampleWidget, parent), ui(new Ui::ExampleWidget)
{
    ui->setupUi(this);
    connect(this, &BaseWidget::cancelSig, this, &BaseWidget::destroySig);
    QTimer::singleShot(0, this, [this]() { emit enableSave(true); });
    connectAllWidgetsToSaveBtn(this);
}

ExampleWidget::~ExampleWidget() { delete ui; }

QStringList ExampleWidget::getTitle() const { return {"example", "title"}; }
