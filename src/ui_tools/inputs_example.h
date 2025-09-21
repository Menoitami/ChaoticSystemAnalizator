#pragma once

#include <QWidget>

namespace Ui
{
class Inputs_example;
}

class Inputs_example : public QWidget
{
    Q_OBJECT

  public:
    explicit Inputs_example(QWidget *parent = nullptr);
    ~Inputs_example();

  private:
    Ui::Inputs_example *ui;
};
