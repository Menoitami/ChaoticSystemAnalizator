#include "settingsbutton.h"

SettingsButton::SettingsButton(QWidget *parent) : AnimatedIconButton(parent)
{
    setIconSize({24, 24});
    setProperty("Variant", "EditButton");
    setSvgIconPath("://Settings");
    setMaximumSize(QSize{24, 24});
}

SettingsButton::~SettingsButton() {}
