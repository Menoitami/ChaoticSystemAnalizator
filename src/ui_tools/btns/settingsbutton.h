#pragma once

#include "animatediconbuttons.h"

#include <QWidget>

class SettingsButton : public AnimatedIconButton
{
    Q_OBJECT

  public:
    explicit SettingsButton(QWidget *parent = nullptr);
    ~SettingsButton();

  private:
};
