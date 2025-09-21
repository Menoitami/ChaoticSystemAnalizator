#pragma once

#include "animatediconbuttons.h"

#include <QWidget>

class DeleteButton : public AnimatedIconButton
{
    Q_OBJECT

  public:
    explicit DeleteButton(QWidget *parent = nullptr);
    ~DeleteButton();

  private:
};
