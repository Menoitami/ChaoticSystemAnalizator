#pragma once

#include "animatediconbuttons.h"

#include <QWidget>

class EditButton : public AnimatedIconButton
{
    Q_OBJECT

  public:
    explicit EditButton(QWidget *parent = nullptr);
    ~EditButton();

  private:
};
