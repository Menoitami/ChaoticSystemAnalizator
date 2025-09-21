#pragma once

#include "basewidget.h"
#include <QHBoxLayout>
#include <QPoint>
#include <QPushButton>
#include <QWidget>
#include <memory>

namespace Ui
{
class TemplateFieldWidget;
}

class TemplateFieldWidget : public QWidget, public std::enable_shared_from_this<TemplateFieldWidget>
{
    /*
     * содержит шапку и кнопки "сохранить" и "отмена" для field_window_widgets
     */

    Q_OBJECT

  public:
    explicit TemplateFieldWidget(QWidget *parent = nullptr);
    ~TemplateFieldWidget();

    void setBaseWidget(std::shared_ptr<BaseWidget>);
    void addBottomLayoutButton(QPushButton *btn);
    QPushButton *addResetButton();
    QSize baseMinimumSize() const;
    QSize getSize() const { return this->size(); };
    std::shared_ptr<BaseWidget> baseWidget() { return wid; }

    void setTitle(QStringList titleParts);

    QPushButton *getCancelButton();
    QPushButton *getSaveButton();

  public slots:
    void enableSaveBtn(bool enable);

  private slots:
    void startDrag();
    void destroyWidgetSlot();

  private:
    Ui::TemplateFieldWidget *ui;

    QPoint position;
    bool isDragging = false;
    bool isResizing = false;

    std::shared_ptr<BaseWidget> wid;

    QPoint resizePosition;
    QRect originalRect;
    int resizeDir;

    void determineResizeDirection(const QPoint &pos);
    void updateCursor(const QPoint &pos);

    QPoint lastResizePos;

    QSize minSize;

    enum ResizeDirection
    {
        ResizeNone = 0x00,
        ResizeLeft = 0x01,
        ResizeRight = 0x02,
        ResizeTop = 0x04,
        ResizeBottom = 0x08
    };

  protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    QSize minimumSizeHint() const override;
    bool eventFilter(QObject *obj, QEvent *event) override;

  signals:
    void destroyWidget(std::shared_ptr<TemplateFieldWidget> widget);

    void cancelSig();
    void saveSig();

    void pressedDrag(std::shared_ptr<TemplateFieldWidget>);
    void dragging(std::shared_ptr<TemplateFieldWidget>, const QPoint &newPos, const QSize &newSize);
    void moveRelease(std::shared_ptr<TemplateFieldWidget>, const QPoint &newPos);
    void resizeRelease(std::shared_ptr<TemplateFieldWidget>, const QPoint &newPos, const QSize &newSize);

    void openWidget(BaseWidget *wid);
};
