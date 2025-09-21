#include "style_tools.h"

#include <QFile>
#include <QGraphicsDropShadowEffect>

void makeShadow(QWidget *wid)
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(12);
    shadow->setXOffset(2);
    shadow->setYOffset(1);
    shadow->setColor(QColor(0, 0, 0, 40));

    wid->setGraphicsEffect(shadow);
}
