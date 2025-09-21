#pragma once

#include <QWidget>

namespace Ui
{
class TripleButtonsDialog;
}

class TripleButtonsDialog : public QWidget
{
    Q_OBJECT

    /*
     Детали реализации. см dialogwidget.h

     */

  public:
    explicit TripleButtonsDialog(QWidget *parent = nullptr);
    ~TripleButtonsDialog();
    void addContentWidget(QWidget *widget);

    void setHeaderText(const QString &text);

  public:
    Ui::TripleButtonsDialog *ui;

  private:
    void centerWidget();

    void paintEvent(QPaintEvent *event) override;
};
