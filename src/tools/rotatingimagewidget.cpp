#include "rotatingimagewidget.h"
#include <QPainter>
#include <QPropertyAnimation>

RotatingImageWidget::RotatingImageWidget(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void RotatingImageWidget::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    m_showPixmap = false;
    update();
}

void RotatingImageWidget::startRotation()
{
    if (!m_animation)
    {
        m_animation = new QPropertyAnimation(this, "rotation", this);
        m_animation->setLoopCount(-1); // бесконечно
        m_animation->setEasingCurve(QEasingCurve::Linear);
    }

    m_animation->setDuration(1000);
    m_animation->setStartValue(m_rotation);
    m_animation->setEndValue(m_rotation + 360); // один полный оборот

    m_showPixmap = true;
    m_animation->start();
}

void RotatingImageWidget::stopRotation()
{
    if (m_animation && m_animation->state() == QPropertyAnimation::Running)
    {
        m_animation->stop();
    }
    m_showPixmap = false;
    update();
}

void RotatingImageWidget::setRotation(qreal angle)
{
    m_rotation = angle;
    update();
}

void RotatingImageWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (m_pixmap.isNull() || !m_showPixmap)
        return; // не рисуем, если pixmap пуст или скрыт

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPointF center = rect().center();
    painter.translate(center);
    painter.rotate(m_rotation);
    painter.translate(-center);

    QRect targetRect = rect();
    painter.drawPixmap(targetRect, m_pixmap, m_pixmap.rect());
}
