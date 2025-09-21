#pragma once

#include <QWidget>

namespace Ui
{
class buttons_example;
}

class ButtonsExample : public QWidget
{
    Q_OBJECT

    /*
     * можно отрисовать чтоб посмотреть на все шаблоны кнопок
     * ButtonsExample be;
     * be.show();
     */

  public:
    explicit ButtonsExample(QWidget *parent = nullptr);
    ~ButtonsExample();

  private:
    Ui::buttons_example *ui;
};
