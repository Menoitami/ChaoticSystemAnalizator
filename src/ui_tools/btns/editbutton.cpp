#include "editbutton.h"

EditButton::EditButton(QWidget *parent) : AnimatedIconButton(parent)
{
    setIconSize({20, 20});
    setProperty("Variant", "EditButton");
    setSvgIconPath("://edit_20");
}

EditButton::~EditButton() {}
