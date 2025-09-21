#pragma once

#include <QLabel>
#include <QTimer>
#include <QWidget>
namespace Ui
{
class CustomToolTip;
}

class CustomToolTip : public QWidget
{
    Q_OBJECT

  public:
    explicit CustomToolTip(QWidget *parent = nullptr);

    void setToolTipTo(QWidget *widget);
    void addText(const QString &text);
    void setText(const QString &text);
    void showAt(const QPoint &pos, const QString &text);
    ~CustomToolTip();

  private:
    Ui::CustomToolTip *ui;

    QLabel *lbl = nullptr;
    QList<QWidget *> wids;
    QWidget *trackedWidget = nullptr;

    QTimer *showTimer;

    void showToolTip(QWidget *widget);
    void hideToolTip(QWidget *widget);

  protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};
