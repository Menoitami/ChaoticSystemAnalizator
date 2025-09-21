#include "closebutton.h"

CloseButton::CloseButton(QWidget *parent) : AnimatedIconButton(parent)
{
    setIconSize({24, 24});
    setProperty("Variant", "CloseButton");
    setSvgIconPath(":Close_24");
}

CloseButton::~CloseButton() {}
