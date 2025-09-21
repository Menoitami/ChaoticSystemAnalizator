#include "copybutton.h"

CopyButton::CopyButton(QWidget *parent) : AnimatedIconButton(parent)
{
    // setIconSize({20, 20});
    setProperty("Variant", "EditButton");
    setSvgIconPath("://copy");
}

CopyButton::~CopyButton() {}
