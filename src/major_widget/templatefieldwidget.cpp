#include "templatefieldwidget.h"
#include "style_tools.h"
#include "stylesheetprocessor.h"
#include "ui_templatefieldwidget.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>

TemplateFieldWidget::TemplateFieldWidget(QWidget *parent) : QWidget(parent), ui(new Ui::TemplateFieldWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    StyleSheetProcessor::applyStyle(":/styles/specific/TemplateFieldWidget.qss", this);

    connect(ui->CancelButton, &QPushButton::clicked, this, &TemplateFieldWidget::cancelSig, Qt::QueuedConnection);
    connect(ui->SaveButton, &QPushButton::clicked, this, &TemplateFieldWidget::saveSig, Qt::QueuedConnection);
    connect(ui->closeButton, &QPushButton::pressed, this, &TemplateFieldWidget::destroyWidgetSlot);
    ui->SaveButton->setEnabled(false);

    ui->verticalLayout_2->setAlignment(Qt::AlignTop);
    ui->verticalLayout_3->setAlignment(Qt::AlignTop);
    //makeShadow(this);
    setMouseTracking(true);
    ui->HeadingH3->installEventFilter(this);
}

TemplateFieldWidget::~TemplateFieldWidget()
{
    disconnect(this, nullptr, wid.get(), nullptr);
    disconnect(wid.get(), nullptr, this, nullptr);
    wid.reset();
    delete ui;
}

void TemplateFieldWidget::setBaseWidget(std::shared_ptr<BaseWidget> ptr)
{
    qDebug() << __FUNCTION__;

    setTitle(ptr->getTitle());
    QLayoutItem *item = ui->horizontalLayout_2->takeAt(2);
    if (item)
    {
        if (item->widget())
        {
            delete item->widget();
        }
        delete item;
    }
    ui->horizontalLayout_2->insertWidget(1, ptr.get());
    this->adjustSize();
    update();
    wid = ptr;

    connect(this, &TemplateFieldWidget::cancelSig, ptr.get(), &BaseWidget::cancelSig);
    connect(this, &TemplateFieldWidget::saveSig, ptr.get(), &BaseWidget::saveSig);
    connect(ptr.get(), &BaseWidget::openWidget, this, &TemplateFieldWidget::openWidget);
    connect(ptr.get(), &BaseWidget::enableSave, this, &TemplateFieldWidget::enableSaveBtn);
    connect(ptr.get(), &BaseWidget::destroySig, this,
            [weakThis = weak_from_this()]()
            {
                if (auto self = weakThis.lock())
                {
                    emit self->destroyWidget(self);
                }
            });

    connect(ptr.get(), &BaseWidget::resizeRequest, this, [this](const QPoint &newPos, const QSize &newSize)
            { emit resizeRelease(shared_from_this(), newPos, newSize); });

    qDebug() << (this->size() - wid->size());
    minSize = baseMinimumSize();
}

QSize TemplateFieldWidget::baseMinimumSize() const
{
    return wid->minimumSize() +
           QSize(this->layout()->contentsMargins().left() + this->layout()->contentsMargins().right(),
                 this->layout()->contentsMargins().top() + this->layout()->contentsMargins().bottom() +
                     ui->CancelButton->height() + ui->HeadingH3->height());
};

void TemplateFieldWidget::startDrag()
{
    if (!isDragging)
        emit pressedDrag(shared_from_this());
    isDragging = true;
    resizePosition = QCursor::pos() - pos();
    this->raise();
    setCursor(Qt::ClosedHandCursor);
}

bool TemplateFieldWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->HeadingH3)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                startDrag();
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (isDragging && (mouseEvent->buttons() & Qt::LeftButton))
            {
                QPoint newPos = QCursor::pos() - resizePosition;
                QWidget *parentWidget = static_cast<QWidget *>(parent());
                if (parentWidget)
                {
                    int maxX = parentWidget->width() - width();
                    int maxY = parentWidget->height() - height();
                    newPos.setX(qBound(0, newPos.x(), maxX));
                    newPos.setY(qBound(0, newPos.y(), maxY));
                }
                move(newPos);

                emit dragging(shared_from_this(), pos(), getSize());
                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            if (isDragging)
            {
                isDragging = false;
                position = pos();
                emit moveRelease(shared_from_this(), position);
                unsetCursor();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void TemplateFieldWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (wid && !wid->canResize)
            return;

        determineResizeDirection(event->pos());
        if (resizeDir != ResizeNone)
        {
            if (!isResizing)
                emit pressedDrag(shared_from_this());
            isResizing = true;
            originalRect = geometry();
            resizePosition = event->pos();
        }
        event->accept();
    }
}

void TemplateFieldWidget::destroyWidgetSlot() { emit destroyWidget(shared_from_this()); }

void TemplateFieldWidget::mouseMoveEvent(QMouseEvent *event)
{
    // if (!isResizing && !isDragging)
    // {
    //     updateCursor(event->pos());
    //     return;
    // }

    if (isResizing)
    {

        updateCursor(event->pos());
        QPoint delta = event->pos() - resizePosition;

        QRect newGeometry = originalRect;

        // Обработка изменения ширины
        if (resizeDir & ResizeLeft)
        {
            int newWidth = qMax(geometry().width() - delta.x(), minimumWidth());
            newGeometry.setX(geometry().x() + delta.x());
            newGeometry.setWidth(newWidth);
        }
        else if (resizeDir & ResizeRight)
        {
            int newWidth = qMax(originalRect.width() + delta.x(), minimumWidth());
            newGeometry.setWidth(newWidth);
        }

        // Обработка изменения высоты
        if (resizeDir & ResizeTop)
        {
            int newHeight = qMax(geometry().height() - delta.y(), minimumHeight());
            newGeometry.setY(geometry().y() + delta.y());
            newGeometry.setHeight(newHeight);
        }
        else if (resizeDir & ResizeBottom)
        {
            int newHeight = qMax(originalRect.height() + delta.y(), minimumHeight());
            newGeometry.setHeight(newHeight);
        }

        if (newGeometry.width() <= minSize.width())
        {
            if (resizeDir & ResizeRight)
                newGeometry.setX(this->x());
            if (resizeDir & ResizeLeft)
                newGeometry.setX(this->x() + this->width() - minSize.width());
            newGeometry.setWidth(minSize.width());
        }

        if (newGeometry.height() <= minSize.height())
        {
            if (resizeDir & ResizeBottom)
                newGeometry.setY(this->y());
            if (resizeDir & ResizeTop)
                newGeometry.setY(this->y() + this->height() - minSize.height());
            newGeometry.setHeight(minSize.height());
        }

        setGeometry(newGeometry);
        event->accept();
    }

    if (isDragging && (event->buttons() & Qt::LeftButton))
    {
        position = event->globalPosition().toPoint() - resizePosition;

        QWidget *parentWidget = static_cast<QWidget *>(parent());
        if (parentWidget)
        {
            int maxX = parentWidget->width() - width();
            int maxY = parentWidget->height() - height();

            position.setX(qBound(0, position.x(), maxX));
            position.setY(qBound(0, position.y(), maxY));
        }

        move(position);
        event->accept();
    }

    emit dragging(shared_from_this(), this->pos(), this->getSize());
    updateCursor(event->pos());

    QWidget::mouseMoveEvent(event);
}

void TemplateFieldWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        position = this->pos();
        if (isDragging)
            emit moveRelease(shared_from_this(), position);

        if (isResizing)
        {
            emit resizeRelease(shared_from_this(), position, this->getSize());
        }

        isDragging = false;
        isResizing = false;
        unsetCursor();
        event->accept();
    }
    QWidget::mouseReleaseEvent(event);
}

void TemplateFieldWidget::addBottomLayoutButton(QPushButton *btn) { ui->bottomLayout->insertWidget(0, btn); }

QPushButton *TemplateFieldWidget::addResetButton()
{
    auto btn = new QPushButton("Установить по умолчанию", parentWidget());
    btn->setProperty("ButtonType", "Default");
    btn->setProperty("Size", "M");
    btn->setProperty("Variant", "outline");

    addBottomLayoutButton(btn);

    return btn;
}

QPushButton *TemplateFieldWidget::getCancelButton() { return ui->CancelButton; }

QPushButton *TemplateFieldWidget::getSaveButton() { return ui->SaveButton; }

void TemplateFieldWidget::determineResizeDirection(const QPoint &pos)
{
    resizeDir = ResizeNone;
    QRect r = rect();
    const int margin = 5;

    bool left = pos.x() <= margin;
    bool right = pos.x() >= r.width() - margin;
    bool top = pos.y() <= margin;
    bool bottom = pos.y() >= r.height() - margin;

    if (left)
        resizeDir |= ResizeLeft;
    if (right)
        resizeDir |= ResizeRight;
    if (top)
        resizeDir |= ResizeTop;
    if (bottom)
        resizeDir |= ResizeBottom;
}

void TemplateFieldWidget::updateCursor(const QPoint &pos)
{
    const int margin = 5;
    QRect r = rect();

    if (r.height() - pos.y() <= margin && r.width() - pos.x() <= margin)
        setCursor(Qt::SizeFDiagCursor); // Нижний правый угол
    else if (pos.y() <= margin && r.width() - pos.x() <= margin)
        setCursor(Qt::SizeBDiagCursor); // Верхний правый угол
    else if (pos.x() <= margin && r.height() - pos.y() <= margin)
        setCursor(Qt::SizeBDiagCursor); // Нижний левый угол
    else if (pos.x() <= margin && pos.y() <= margin)
        setCursor(Qt::SizeFDiagCursor); // Верхний левый угол
    else if (r.width() - pos.x() <= margin || pos.x() <= margin)
        setCursor(Qt::SizeHorCursor); // Горизонтальные края
    else if (r.height() - pos.y() <= margin || pos.y() <= margin)
        setCursor(Qt::SizeVerCursor); // Вертикальные края
    else
        unsetCursor(); // Сброс до стандартного
}

QSize TemplateFieldWidget::minimumSizeHint() const { return minSize; }

void TemplateFieldWidget::enableSaveBtn(bool enable) { ui->SaveButton->setEnabled(enable); }

void TemplateFieldWidget::setTitle(QStringList titleParts)
{
    if (!titleParts.isEmpty())
    {
        QString html;

        for (int i = 0; i < titleParts.size() - 1; ++i)
        {
            html += QString("<span style='color: #6A7384;'>%1 / </span>").arg(titleParts[i]);
        }
        html += QString("<span style='color: #4882EE;'>%1</span>").arg(titleParts.last());

        ui->HeadingH3->setText(html);
    }
    else
    {
        ui->HeadingH3->setText("");
    }
}
