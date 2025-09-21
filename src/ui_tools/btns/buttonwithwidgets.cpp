#include "buttonwithwidgets.h"
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

ButtonWithWidgets::ButtonWithWidgets(QWidget *parent)
    : QWidget(parent), m_layout(nullptr), m_textLabel(nullptr), m_pressed(false), m_hovered(false)
{
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, true);

    if (layout())
        m_layout = layout();
    else
    {
        qWarning() << parent->metaObject()->className()
                   << "No layout set in .ui — please assign a layout in Qt Designer!";
        addLayout();
        return;
    }
}
ButtonWithWidgets::~ButtonWithWidgets()
{
    // Виджеты удалит Qt автоматически (они — дети this)
}

void ButtonWithWidgets::addWidget(QWidget *widget)
{
    if (!widget)
        return;

    if (!m_layout)
    {
        addLayout();
    }
    widget->setParent(this);
    m_layout->addWidget(widget);
    m_widgets.append(widget);
    widget->show();
    widget->raise();

    // Если это QLabel — запоминаем, чтобы setText работал
    if (qobject_cast<QLabel *>(widget) && !m_textLabel)
    {
        m_textLabel = static_cast<QLabel *>(widget);
    }
}

const QList<QWidget *> &ButtonWithWidgets::widgets() const { return m_widgets; }

void ButtonWithWidgets::setLayout(QLayout *layout)
{
    if (!layout)
        return;

    m_layout = layout;
    QWidget::setLayout(layout);
}

void ButtonWithWidgets::addLayout()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(6, 6, 6, 6);
    m_layout->setSpacing(6);
    setLayout(m_layout);
}

void ButtonWithWidgets::setText(const QString &text)
{
    if (m_textLabel)
    {
        m_textLabel->setText(text);
    }
}

void ButtonWithWidgets::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pressed = true;
        update();
    }
    QWidget::mousePressEvent(event);
}

void ButtonWithWidgets::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pressed = false;
        update();

        if (rect().contains(event->pos()))
        {
            emit clicked();
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void ButtonWithWidgets::enterEvent(QEvent *event)
{
    m_hovered = true;
    update();
}

void ButtonWithWidgets::leaveEvent(QEvent *event)
{
    m_hovered = false;
    m_pressed = false;
    update();
    QWidget::leaveEvent(event);
}

void ButtonWithWidgets::resizeEvent(QResizeEvent *event) { QWidget::resizeEvent(event); }

void ButtonWithWidgets::paintEvent(QPaintEvent *event)
{

    QStyleOptionButton option;
    option.initFrom(this);

    if (m_hovered)
        option.state |= QStyle::State_MouseOver;

    if (m_pressed)
        option.state |= QStyle::State_Sunken;

    if (isEnabled())
        option.state |= QStyle::State_Enabled;
    else
        option.state &= ~QStyle::State_Enabled;

    if (hasFocus())
        option.state |= QStyle::State_HasFocus;

    QPainter painter(this);
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}
