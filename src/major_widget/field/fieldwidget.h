#pragma once

#include "basewidget.h"
#include "constants.hpp"
#include "templatefieldwidget.h"
#include <QDebug>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>

namespace Ui
{
class FieldWidget;
}

using WidgetCreator = std::function<std::shared_ptr<BaseWidget>(QWidget *)>;

class FieldWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit FieldWidget(QWidget *parent = nullptr);
    ~FieldWidget();

    // Регистрация пользовательских виджетов
    template <class T> static void registerWidget(const  SA::CustomWidgets name);

  public slots:

    std::shared_ptr<TemplateFieldWidget> createWidget( SA::CustomWidgets);

    void destroyWidget(std::shared_ptr<TemplateFieldWidget> widget);

    // Позиционирование виджетов
    void placeInGrid(std::shared_ptr<TemplateFieldWidget> widget);
    void placeInGrid(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newPos);

    // Обработка выбора позиции
    void choosePlace(std::shared_ptr<TemplateFieldWidget>, const QPoint &newPos, const QSize &newSize);

    // Обработка перемещения
    void widgetStartMove(std::shared_ptr<TemplateFieldWidget>);

    void resizeRelease(std::shared_ptr<TemplateFieldWidget>, const QPoint &newPos, const QSize &newSize);

  private:
    // UI элементы
    Ui::FieldWidget *ui;

    // Параметры сетки
    static constexpr quint8 GRID_COLS = 8;
    static constexpr quint8 GRID_ROWS = 4;
    static constexpr quint8 CELL_MARGIN = 6;
    std::shared_ptr<TemplateFieldWidget> grid[GRID_ROWS][GRID_COLS] = {};
    std::shared_ptr<TemplateFieldWidget> testGrid[GRID_ROWS][GRID_COLS] = {};

    QRect findWidgetRectInGrid(std::shared_ptr<TemplateFieldWidget> widget,
                               std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS]);

    // Активные виджеты
    QVector<std::shared_ptr<TemplateFieldWidget>> activeWidgets;

    // Геометрия ячеек
    quint16 cellWidth;
    quint16 cellHeight;

    // Методы работы с сеткой
    void clearGrid();
    void copyGridToTest();

    // Добавление/удаление из сетки
    bool addWidgetToGrid(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &widPosInCells,
                         const QSize &overlapSizeInCells,
                         std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS]);
    bool removeWidgetFromGrid(std::shared_ptr<TemplateFieldWidget> widget,
                              std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS]);

    // Логика позиционирования
    QPoint findFreeCell(int widthInCells, int heightInCells,
                        std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS]);
    QPoint findNearbyFreeCell(int widthInCells, int heightInCells, QPoint startCell,
                              std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS]);

    QPoint calculateGridPosition(const QPoint &widgetPos, const QSize &widgetSizeInCells);

    // Обработка перекрытий
    QList<QPoint> findWidgetCells(std::shared_ptr<TemplateFieldWidget> widget);
    void calculateShifts(std::shared_ptr<TemplateFieldWidget> widget,
                         const QList<std::shared_ptr<TemplateFieldWidget>> &overlappingWidgets, const QPoint &newPos);
    void isOverlapping(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newPos);

    // Перемещение виджетов
    bool moveWidgetInGrid(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newCellPos);

    // Временные данные для перекрытий
    QList<QPoint> newOverLapPos;
    QList<std::shared_ptr<TemplateFieldWidget>> overlappingWidgets;
    bool overlapResolved;

    // размерчики
    QSize sizeInCells(const QSize &size) const;
    void setWidgetSizeInCells(std::shared_ptr<TemplateFieldWidget> widget, const QSize &sizeInCells);

    void resizeAndMoveWithAnimation(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newPos,
                                    const QSize &newSize);

    // Регистрация виджетов
    static QMap< SA::CustomWidgets, WidgetCreator> creators;

    // style
    quint8 sideMargin;
    quint8 bottomMargin;

  protected:
    // Обработка событий
    void resizeEvent(QResizeEvent *event) override;

  signals:
    // Сигналы (если нужны)
};

template <class T> void FieldWidget::registerWidget(const  SA::CustomWidgets name)
{

    creators[name] = [](QWidget *parent) { return std::make_shared<T>(parent); };
}
