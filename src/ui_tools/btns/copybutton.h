#pragma once

#include "animatediconbuttons.h"

#include <QWidget>

class CopyButton : public AnimatedIconButton
{
    Q_OBJECT

  public:
    explicit CopyButton(QWidget *parent = nullptr);
    ~CopyButton();

  private:
};
