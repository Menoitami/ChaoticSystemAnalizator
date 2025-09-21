#pragma once

#include "animatediconbuttons.h"

#include <QWidget>

class CloseButton : public AnimatedIconButton
{
    Q_OBJECT

  public:
    explicit CloseButton(QWidget *parent = nullptr);
    ~CloseButton();

  private:
};
