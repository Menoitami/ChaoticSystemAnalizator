#include "deletebutton.h"

DeleteButton::DeleteButton(QWidget *parent) : AnimatedIconButton(parent)
{
    // setIconSize({20, 20});
    setProperty("Variant", "EditButton");
    setSvgIconPath("://trash_20");
}

DeleteButton::~DeleteButton() {}
