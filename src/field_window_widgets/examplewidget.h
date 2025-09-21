#pragma once

#include "basewidget.h"
#include <QWidget>

namespace Ui
{
class ExampleWidget;
}

class ExampleWidget : public BaseWidget
{
    Q_OBJECT

  public:
    explicit ExampleWidget(QWidget *parent = nullptr);
    ~ExampleWidget();

    QStringList getTitle() const override;

  private:
    Ui::ExampleWidget *ui;
};
