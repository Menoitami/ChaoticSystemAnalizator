#pragma once

#include "triplebuttonsdialog.h"
#include <QWidget>
#include <memory>

class ShadedTripleWidget : public QWidget
{
    /*
     * класс управляет затенением всего окна (см поле overlay) и отрисовкой поверх CustomDialog.
     * CustomDialog по умолчанию содержит отцентрованный заголовок и две кнопки (noButton и yesButton), а также
     * contentLayout.
     *
     * рекомендую для создания окна унаследоваться от этого класса.
     * чтоб менять внешний вид кнопок можно обращаться к dialog->ui
     */

    Q_OBJECT
  public:
    ShadedTripleWidget(QWidget *parent = nullptr);
    virtual ~ShadedTripleWidget() {}

    // добавляет в contentLayout передаваемый виджет
    virtual void addContentWidget(QWidget *widget);

    virtual void setHeaderText(const QString &text);

    void setShadeCoeff(int coeff);

  signals:
    void cancel();
    void accepted();
    void rejected();

  protected:
    std::unique_ptr<TripleButtonsDialog> dialog;
    std::unique_ptr<QWidget> overlay;
    int shadeCoeff = 30;

  protected:
    void shadeWindow();

    void paintEvent(QPaintEvent *event) override;
};
