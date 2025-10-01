#pragma once

#include "ConnectionTypes.hpp"
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
    SystemData schemeData;

  private slots:

    void tryToSetSystem();

  public slots:

    void setMessage(const QString &message);

  signals:

    void setSystem(SystemData &);
};
