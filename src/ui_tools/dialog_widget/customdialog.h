#pragma once

#include <QWidget>

namespace Ui
{
class CustomDialog;
}

class CustomDialog : public QWidget
{
    Q_OBJECT

    /*
     Детали реализации. см dialogwidget.h

     */

  public:
    explicit CustomDialog(QWidget *parent = nullptr);
    ~CustomDialog();
    void addContentWidget(QWidget *widget);
    void addContentSubLayout(QLayout *layout);

    void setHeaderText(const QString &text);

  public:
    Ui::CustomDialog *ui;

  private:
    void centerWidget();

    void paintEvent(QPaintEvent *event) override;
};
