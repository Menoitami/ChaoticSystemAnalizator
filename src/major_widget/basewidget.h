#pragma once

#include "constants.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>

namespace Ui
{
}

class BaseWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit BaseWidget(SA::CustomWidgets name, QWidget *parent = nullptr);
    SA::CustomWidgets getName() const { return name; };
    virtual ~BaseWidget();

    virtual QStringList getTitle() const = 0;
    bool canResize = true;

  protected:
    SA::CustomWidgets name;

    void connectAllWidgetsToSaveBtn(QWidget *widgetContainer);

    template <typename Func> void addResetButton(QWidget *parent, QObject *context, Func onReset);

  private:
  signals:

    void destroySig();
    void enableSave(bool st);

    void saveSig();
    void cancelSig();

    void resizeRequest(const QPoint &newPos, const QSize &newSize);

    void openWidget(BaseWidget *wid);

    void startLoading();
    void stopLoading();
};
