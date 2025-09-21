#include "customtooltip.h"
#include "style_tools.h"
#include "ui_customtooltip.h"
#include <QScreen>
#include <QTimer>

CustomToolTip::CustomToolTip(QWidget *parent) : QWidget(parent), ui(new Ui::CustomToolTip)
{
    ui->setupUi(this);
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    lbl = new QLabel(this);
    lbl->setWordWrap(false);
    lbl->setStyleSheet("color: #4882EE; font-family: Inter; font-size: 14px;");

    ui->widget->setStyleSheet(
        "#widget {background-color: white;border: 1px solid white;border-radius:4px;padding: 8px 4px;}");

    ui->widget->layout()->addWidget(lbl);

    showTimer = new QTimer(this);
    showTimer->setSingleShot(true);
    showTimer->setInterval(500);

    connect(showTimer, &QTimer::timeout, this,
            [this]()
            {
                if (!trackedWidget)
                    return;

                if (trackedWidget->underMouse())
                {
                    showToolTip(trackedWidget);
                }
                else
                {
                    trackedWidget = nullptr;
                }
            });

    this->hide();

    makeShadow(this);
}

CustomToolTip::~CustomToolTip() { delete ui; }

void CustomToolTip::addText(const QString &text)
{
    if (lbl)
    {
        lbl->setText(lbl->text() + text);
    }
}

void CustomToolTip ::setText(const QString &text)
{
    if (lbl)
    {
        lbl->setText(text);
    }
}

void CustomToolTip::setToolTipTo(QWidget *widget)
{

    if (!widget)
        return;

    if (wids.contains(widget))
        return;

    wids.append(widget);

    widget->installEventFilter(this);
}

void CustomToolTip::showToolTip(QWidget *widget)
{

    if (!wids.contains(widget))
        return;

    QPoint pos = QCursor::pos() + QPoint(5, -ui->widget->height() - 5);

    this->move(pos);
    this->show();
    this->adjustSize();
    this->raise();
    this->activateWindow();
}

void CustomToolTip::hideToolTip(QWidget *widget)
{
    if (!widget->underMouse())
        this->hide();
}

bool CustomToolTip::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget *>(watched);
    if (!widget || !wids.contains(widget))
        return false;

    if (event->type() == QEvent::Enter)
    {
        trackedWidget = widget;
        showTimer->start();
    }
    else if (event->type() == QEvent::Leave)
    {
        showTimer->stop();
        trackedWidget = nullptr;

        if (this->isVisible() && !this->underMouse())
        {
            this->hide();
        }
    }

    return false;
}

void CustomToolTip::leaveEvent(QEvent *event)
{
    hide();
    QWidget::leaveEvent(event);
}
