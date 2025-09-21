#pragma once

#include "basewidget.h"
#include "differencescheme.h"
#include <QWidget>

namespace Ui
{
class SystemSettings;
}

class SystemSettings : public BaseWidget
{
    Q_OBJECT

  public:
    explicit SystemSettings(QWidget *parent = nullptr);
    ~SystemSettings();

    QStringList getTitle() const override;

  private:
    Ui::SystemSettings *ui;
    SA::SystemData schemeData;

  private slots:

    void tryToSetSystem();

  signals:

    void setSystem(SA::SystemData &);
};
