#pragma once

#include "customdialog.h"
#include <QWidget>
#include <memory>

class ShadedDialogWidget : public QWidget
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
    ShadedDialogWidget(QWidget *parent = nullptr);
    virtual ~ShadedDialogWidget() {}

    // добавляет в contentLayout передаваемый виджет
    virtual void addContentWidget(QWidget *widget);

    virtual void setHeaderText(const QString &text);
    void setWindowSize(const QSize &size) { dialog->setFixedSize(size); }

    void setShadeCoeff(int coeff);
    void setSaveEnabled(bool);

  signals:
    void accepted();
    void rejected();

  protected:
    std::unique_ptr<CustomDialog> dialog;
    std::unique_ptr<QWidget> overlay;
    int shadeCoeff = 30;

  protected:
    void shadeWindow();

    void paintEvent(QPaintEvent *event) override;
};
