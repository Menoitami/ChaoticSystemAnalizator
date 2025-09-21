#include "fieldwidget.h"
#include "ui_fieldwidget.h"

QMap<SA::CustomWidgets, WidgetCreator> FieldWidget::creators;

FieldWidget::FieldWidget(QWidget *parent) : QWidget(parent), ui(new Ui::FieldWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);

    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_StyledBackground);

    clearGrid();
    update();
}

FieldWidget::~FieldWidget() { delete ui; }

std::shared_ptr<TemplateFieldWidget> FieldWidget::createWidget(SA::CustomWidgets name)
{
    if (creators.contains(name))
    {
        auto templateWidget = std::make_shared<TemplateFieldWidget>(this);

        templateWidget->setBaseWidget(creators[name](templateWidget.get()));
        placeInGrid(templateWidget);

        templateWidget->show();
        connect(templateWidget.get(), &TemplateFieldWidget::destroyWidget, this, &FieldWidget::destroyWidget);
        connect(templateWidget.get(), &TemplateFieldWidget::dragging, this, &FieldWidget::choosePlace);
        connect(templateWidget.get(), &TemplateFieldWidget::moveRelease, this,
                static_cast<void (FieldWidget::*)(std::shared_ptr<TemplateFieldWidget>, const QPoint &)>(
                    &FieldWidget::placeInGrid));
        connect(templateWidget.get(), &TemplateFieldWidget::pressedDrag, this, &FieldWidget::widgetStartMove);
        connect(templateWidget.get(), &TemplateFieldWidget::resizeRelease, this, &FieldWidget::resizeRelease);

        return templateWidget;
    }
    else
    {
        qWarning() << "Error: No registered creator";
    }
    return nullptr;
}

void FieldWidget::destroyWidget(std::shared_ptr<TemplateFieldWidget> widget)
{
    if (!widget)
        qCritical() << __FUNCTION__ << " passed nullptr TemplateFieldWidget";

    QList<QPoint> oldCells = findWidgetCells(widget);

    removeWidgetFromGrid(widget, grid);
    removeWidgetFromGrid(widget, testGrid);
    overlappingWidgets.clear();

    auto item = std::find(activeWidgets.begin(), activeWidgets.end(), widget);
    if (item != activeWidgets.end())
    {
        activeWidgets.erase(item);
    }
    else
        qCritical() << __FUNCTION__ << " Template widget not found";

    widget.reset();
}

QPoint FieldWidget::findFreeCell(int widthInCells, int heightInCells,
                                 std::shared_ptr<TemplateFieldWidget> (&grid_)[GRID_ROWS][GRID_COLS])
{
    for (int row = 0; row <= GRID_ROWS - heightInCells; ++row)
    {
        for (int col = 0; col <= GRID_COLS - widthInCells; ++col)
        {
            bool spaceAvailable = true;

            for (int w = 0; w < widthInCells && spaceAvailable; ++w)
            {
                for (int h = 0; h < heightInCells && spaceAvailable; ++h)
                {
                    if (grid_[row + h][col + w])
                    {
                        spaceAvailable = false;
                    }
                }
            }

            if (spaceAvailable)
            {
                return QPoint(col, row);
            }
        }
    }

    return QPoint(-1, -1); // Не найдено места
}

QPoint FieldWidget::findNearbyFreeCell(int widthInCells, int heightInCells, QPoint startCell,
                                       std::shared_ptr<TemplateFieldWidget> (&grid_)[GRID_ROWS][GRID_COLS])
{
    if (widthInCells < 1 || heightInCells < 1)
    {
        qWarning() << "Invalid cell size";
        return QPoint(-1, -1);
    }

    const int maxRadius = qMax(GRID_COLS, GRID_ROWS);

    const QPoint directions[4] = {
        QPoint(1, 0),  // Right
        QPoint(0, 1),  // Down
        QPoint(-1, 0), // Left
        QPoint(0, -1)  // Up
    };

    // поиск спиралькой
    for (int r = 0; r < maxRadius; ++r)
    {
        for (int dirIndex = 1; dirIndex < 7; ++dirIndex)
        {
            int dir = dirIndex % 4;
            const QPoint &direction = directions[dir];

            int steps = r + 1;

            for (int step = 0; step < steps; ++step)
            {
                int col = startCell.x();
                int row = startCell.y();

                col += direction.x() * step;
                row += direction.y() * step;

                if (col < 0 || row < 0 || col + widthInCells > GRID_COLS || row + heightInCells > GRID_ROWS)
                    continue;

                bool spaceAvailable = true;
                for (int dx = 0; dx < widthInCells && spaceAvailable; ++dx)
                {
                    for (int dy = 0; dy < heightInCells && spaceAvailable; ++dy)
                    {
                        if (grid_[row + dy][col + dx])
                            spaceAvailable = false;
                    }
                }

                if (spaceAvailable)
                {
                    return QPoint(col, row);
                }
            }
        }
    }
    return findFreeCell(widthInCells, heightInCells, grid_);
}

QSize FieldWidget::sizeInCells(const QSize &size) const
{
    if (cellWidth <= 0 || cellHeight <= 0)
    {
        qWarning() << "Cell size is not initialized";
        return QSize(0, 0);
    }

    // Округление вверх - даже частичная ячейка требует новой ячейки
    int widthInCells = qCeil(static_cast<double>(size.width()) / cellWidth);
    int heightInCells = qCeil(static_cast<double>(size.height()) / cellHeight);

    // Минимальное значение 1 ячейка
    widthInCells = std::max(1, widthInCells);
    heightInCells = std::max(1, heightInCells);

    return QSize(widthInCells, heightInCells);
}

void FieldWidget::setWidgetSizeInCells(std::shared_ptr<TemplateFieldWidget> widget, const QSize &sizeInCells)
{

    if (!widget)
        return;

    QWidget *widgetPtr = widget.get();
    if (!widgetPtr)
        return;

    // Вычисление новых размеров
    int newWidth = (sizeInCells.width() * cellWidth) - CELL_MARGIN * 2;
    int newHeight = (sizeInCells.height() * cellHeight) - CELL_MARGIN * 2;

    // Проверка необходимости анимации
    if (widgetPtr->width() == newWidth && widgetPtr->height() == newHeight)
        return;

    // Анимация размера
    QPropertyAnimation *animation = new QPropertyAnimation(widgetPtr, "size");
    animation->setDuration(300);
    animation->setStartValue(widgetPtr->size());
    animation->setEndValue(QSize(newWidth, newHeight));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

QList<QPoint> FieldWidget::findWidgetCells(std::shared_ptr<TemplateFieldWidget> widget)
{
    QList<QPoint> cells;
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            if (grid[row][col] == widget)
            {
                cells.append(QPoint(col, row));
            }
        }
    }
    return cells;
}

void FieldWidget::placeInGrid(std::shared_ptr<TemplateFieldWidget> widget)
{
    QSize widSize = widget->baseMinimumSize();
    widSize.setWidth(std::max(1.0, std::ceil(widSize.width() / cellWidth) + 1));
    widSize.setHeight(std::max(1.0, std::ceil(widSize.height() / cellHeight) + 1));

    qDebug() << __FUNCTION__ << widSize;

    QPoint freeCell = findFreeCell(widSize.width(), widSize.height(), grid);

    if (freeCell.x() == -1 || freeCell.y() == -1)
    {
        qWarning() << "No space to re-place widget after drag";
        widget->move(-500, -500);
        destroyWidget(widget);
        return;
    }
    resizeAndMoveWithAnimation(widget, freeCell, widSize);

    if (!activeWidgets.contains(widget))
    {
        activeWidgets.push_back(widget);
    }
}

void FieldWidget::placeInGrid(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newPos)
{
    if (!widget)
    {
        qCritical() << __FUNCTION__ << "Passed null widget";
        return;
    }

    if (!overlappingWidgets.isEmpty() && !newOverLapPos.isEmpty() && overlappingWidgets.size() == newOverLapPos.size())
    {
        for (int i = 0; i < overlappingWidgets.size(); ++i)
        {
            auto &overlapWidget = overlappingWidgets[i];
            const QPoint &newPosOverlap = newOverLapPos[i];

            if (overlapWidget)
            {
                moveWidgetInGrid(overlapWidget, newPosOverlap);
            }
        }
    }

    QSize widSize = sizeInCells(widget->getSize());
    QPoint cellPos = calculateGridPosition(newPos, widSize);
    QPoint freeCell = findNearbyFreeCell(widSize.width(), widSize.height(), cellPos, grid);

    moveWidgetInGrid(widget, freeCell);

    if (!activeWidgets.contains(widget))
    {
        activeWidgets.push_back(widget);
    }
}

bool FieldWidget::moveWidgetInGrid(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newCellPos)
{

    if (!widget || cellWidth <= 0 || cellHeight <= 0)
    {
        qCritical() << "Invalid widget or cell size";
        return false;
    }

    const QSize widgetSize = sizeInCells(widget->getSize());

    // Проверка границ с учетом размера виджета
    if (newCellPos.x() + widgetSize.width() > GRID_COLS || newCellPos.y() + widgetSize.height() > GRID_ROWS)
    {
        qWarning() << "Target position is out of grid bounds.";
        return false;
    }

    // Освобождаем старые ячейки
    QList<QPoint> oldCells = findWidgetCells(widget);
    for (const QPoint &cell : oldCells)
    {
        grid[cell.y()][cell.x()] = nullptr;
    }

    // Проверяем, свободно ли место
    for (int dx = 0; dx < widgetSize.width(); ++dx)
    {
        for (int dy = 0; dy < widgetSize.height(); ++dy)
        {
            if (grid[newCellPos.y() + dy][newCellPos.x() + dx] != nullptr)
            {
                qWarning() << "Target cells are occupied.";
                return false;
            }
        }
    }

    // Устанавливаем родителя
    widget->setParent(this);
    widget->raise();

    // Расчет позиции и размеров
    const int newX = newCellPos.x() * cellWidth + CELL_MARGIN;
    const int newY = newCellPos.y() * cellHeight + CELL_MARGIN;
    const int newWidth = widgetSize.width() * cellWidth - CELL_MARGIN * 2;
    const int newHeight = widgetSize.height() * cellHeight - CELL_MARGIN * 2;

    // Устанавливаем размер один раз
    widget->resize(newWidth, newHeight);

    // Сохраняем текущую позицию перед анимацией
    const QPoint startPos = widget->pos();
    const QPoint endPos(newX, newY);

    // Создаем анимацию
    QPropertyAnimation *anim = new QPropertyAnimation(widget.get(), "pos");
    anim->setDuration(300);
    anim->setStartValue(startPos);
    anim->setEndValue(endPos);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    connect(anim, &QPropertyAnimation::finished, anim, &QPropertyAnimation::deleteLater);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    // Обновляем сетку
    for (int dx = 0; dx < widgetSize.width(); ++dx)
    {
        for (int dy = 0; dy < widgetSize.height(); ++dy)
        {
            grid[newCellPos.y() + dy][newCellPos.x() + dx] = widget;
        }
    }

    return true;
}

void FieldWidget::resizeAndMoveWithAnimation(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newCellPos,
                                             const QSize &sizeCell)
{
    if (!widget || cellWidth <= 0 || cellHeight <= 0)
    {
        qCritical() << "Invalid widget or cell size";
        return;
    }

    QWidget *widgetPtr = widget.get();

    // Проверка границ с учетом размера виджета
    int newWidthInCells = sizeCell.width();
    int newHeightInCells = sizeCell.height();

    if (newCellPos.x() + newWidthInCells > GRID_COLS || newCellPos.y() + newHeightInCells > GRID_ROWS)
    {
        qWarning() << "Target position is out of grid bounds.";
        return;
    }

    addWidgetToGrid(widget, newCellPos, QSize(newWidthInCells, newHeightInCells), grid);

    widget->setParent(this);
    widget->raise();

    const int newX = newCellPos.x() * cellWidth + CELL_MARGIN;
    const int newY = newCellPos.y() * cellHeight + CELL_MARGIN;

    const int newWidth = (newWidthInCells * cellWidth) - CELL_MARGIN * 2;
    const int newHeight = (newHeightInCells * cellHeight) - CELL_MARGIN * 2;

    const QRect startRect = widgetPtr->geometry();
    const QRect endRect = QRect(newX, newY, newWidth, newHeight);

    if (startRect == endRect)
        return;

    QPropertyAnimation *animation = new QPropertyAnimation(widgetPtr, "geometry");
    animation->setDuration(300);
    animation->setStartValue(startRect);
    animation->setEndValue(endRect);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void FieldWidget::widgetStartMove(std::shared_ptr<TemplateFieldWidget> widget)
{
    removeWidgetFromGrid(widget, grid);
    widget->raise();
    copyGridToTest();
}

void FieldWidget::choosePlace(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newPos, const QSize &newSize)
{
    if (!widget || cellWidth <= 0 || cellHeight <= 0)
        return;

    const QSize widSize = sizeInCells(widget->getSize());
    QPoint cellPos = calculateGridPosition(newPos, widSize);

    isOverlapping(widget, cellPos);

    if (overlappingWidgets.length() != 0)
    {
        calculateShifts(widget, overlappingWidgets, cellPos);
        overlapResolved = newOverLapPos.length() != 0 ? true : false;
    }
    else
    {
        overlapResolved = true;
    }
}

QPoint FieldWidget::calculateGridPosition(const QPoint &widgetPos, const QSize &widgetSizeInCells)
{
    int col = (widgetPos.x() + cellWidth / 2) / cellWidth;
    int row = (widgetPos.y() + cellHeight / 2) / cellHeight;

    int maxCol = GRID_COLS - widgetSizeInCells.width();
    int maxRow = GRID_ROWS - widgetSizeInCells.height();

    col = qMin(col, maxCol);
    row = qMin(row, maxRow);

    col = qMax(col, 0);
    row = qMax(row, 0);

    return QPoint(col, row);
}

void FieldWidget::isOverlapping(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &cellPos)
{
    overlappingWidgets.clear();

    if (!widget || cellWidth <= 0 || cellHeight <= 0)
        return;

    const QSize widSize = sizeInCells(widget->getSize());

    for (int dx = 0; dx < widSize.width(); ++dx)
    {
        for (int dy = 0; dy < widSize.height(); ++dy)
        {
            int col = cellPos.x() + dx;
            int row = cellPos.y() + dy;

            if (col < 0 || col >= GRID_COLS || row < 0 || row >= GRID_ROWS)
                continue;

            auto cellWidget = grid[row][col];
            if (cellWidget && cellWidget != widget)
            {
                if (!overlappingWidgets.contains(cellWidget))
                {
                    overlappingWidgets.append(cellWidget);
                }
            }
        }
    }
}

void FieldWidget::calculateShifts(std::shared_ptr<TemplateFieldWidget> widget,
                                  const QList<std::shared_ptr<TemplateFieldWidget>> &overlappingWidgets,
                                  const QPoint &widPos)
{
    newOverLapPos.clear();
    if (!widget || cellWidth <= 0 || cellHeight <= 0)
        return;

    copyGridToTest();

    const QSize widSize = sizeInCells(widget->getSize());

    for (const auto &overlap : overlappingWidgets)
    {
        removeWidgetFromGrid(overlap, testGrid);
    }

    if (!addWidgetToGrid(widget, widPos, widSize, testGrid))
    {
        return;
    }

    for (const auto &overlap : overlappingWidgets)
    {

        const QSize overLapSize = sizeInCells(overlap->getSize());
        QPoint overLapPos = calculateGridPosition(overlap->pos(), overLapSize);

        QPoint overLapNewPos = findNearbyFreeCell(overLapSize.width(), overLapSize.height(), overLapPos, testGrid);

        if (overLapNewPos == QPoint(-1, -1))
        {
            newOverLapPos.clear();
            return;
        }
        addWidgetToGrid(overlap, overLapNewPos, overLapSize, testGrid);

        newOverLapPos.append(overLapNewPos);
    }
}

bool FieldWidget::addWidgetToGrid(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &widPosInCells,
                                  const QSize &sizeInCells,
                                  std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS])
{
    if (!widget)
        return false;

    int width = sizeInCells.width();
    int height = sizeInCells.height();

    // Check if the widget would go out of bounds
    if (widPosInCells.x() + width > GRID_COLS || widPosInCells.y() + height > GRID_ROWS)
    {
        qWarning() << "Cannot add widget: it exceeds grid boundaries.";
        return false;
    }

    // Check if the target cells are free
    for (int dx = 0; dx < width; ++dx)
    {
        for (int dy = 0; dy < height; ++dy)
        {
            int col = widPosInCells.x() + dx;
            int row = widPosInCells.y() + dy;

            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS)
            {
                if (grid[row][col] != nullptr)
                {
                    qWarning() << "Cell (" << col << "," << row << ") is already occupied.";
                    return false;
                }
            }
        }
    }

    // Add the widget to the grid
    for (int dx = 0; dx < width; ++dx)
    {
        for (int dy = 0; dy < height; ++dy)
        {
            int col = widPosInCells.x() + dx;
            int row = widPosInCells.y() + dy;
            grid[row][col] = widget;
        }
    }

    return true;
}

bool FieldWidget::removeWidgetFromGrid(std::shared_ptr<TemplateFieldWidget> widget,
                                       std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS])
{
    if (!widget)
        return false;

    bool removed = false;

    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            if (grid[row][col] == widget)
            {
                grid[row][col] = nullptr;
                removed = true;
            }
        }
    }

    if (!removed)
    {
        // qWarning() << "Widget not found in the grid.";
        return false;
    }

    return true;
}

void FieldWidget::resizeRelease(std::shared_ptr<TemplateFieldWidget> widget, const QPoint &newPos, const QSize &newSize)
{
    if (!widget)
    {
        qCritical() << __FUNCTION__ << "Passed null widget";
        return;
    }

    if (!overlappingWidgets.isEmpty() && !newOverLapPos.isEmpty() && overlappingWidgets.size() == newOverLapPos.size())
    {
        for (int i = 0; i < overlappingWidgets.size(); ++i)
        {
            auto &overlapWidget = overlappingWidgets[i];
            const QPoint &newPosOverlap = newOverLapPos[i];

            if (overlapWidget)
            {
                moveWidgetInGrid(overlapWidget, newPosOverlap);
            }
        }
    }

    QSize widSize = sizeInCells(newSize);
    QPoint cellPos = calculateGridPosition(newPos, widSize);
    QPoint freeCell;
    removeWidgetFromGrid(widget, testGrid);
    removeWidgetFromGrid(widget, grid);

    auto tryResizeAndPlace = [&](int newWidth, int newHeight) -> bool
    {
        freeCell = findNearbyFreeCell(newWidth, newHeight, cellPos, grid);
        if (freeCell != QPoint(-1, -1))
        {
            resizeAndMoveWithAnimation(widget, freeCell, QSize(newWidth, newHeight));
            return true;
        }
        return false;
    };

    if (tryResizeAndPlace(widSize.width(), widSize.height()))
        return;

    // Попытаться уменьшить только ширину
    QSize minSize = widget->baseMinimumSize();
    minSize.setWidth(std::max(1.0, std::ceil(minSize.width() / cellWidth) + 1));
    minSize.setHeight(std::max(1.0, std::ceil(minSize.height() / cellHeight) + 1));

    int currentWidth = widSize.width();
    int currentHeight = widSize.height();

    for (int newWidth = currentWidth - 1; newWidth >= minSize.width(); --newWidth)
    {
        if (tryResizeAndPlace(newWidth, currentHeight))
            return;
    }

    // Попытаться уменьшить только высоту
    for (int newHeight = currentHeight - 1; newHeight >= minSize.height(); --newHeight)
    {
        if (tryResizeAndPlace(currentWidth, newHeight))
            return;
    }

    // Одновременное уменьшение ширины и высоты
    while (currentWidth > minSize.width() || currentHeight > minSize.height())
    {
        int deltaW = std::min(currentWidth - minSize.width(), 1);
        int deltaH = std::min(currentHeight - minSize.height(), 1);
        int newWidth = currentWidth - deltaW;
        int newHeight = currentHeight - deltaH;

        if (tryResizeAndPlace(newWidth, newHeight))
            return;

        currentWidth = newWidth;
        currentHeight = newHeight;
    }

    // Если всё не сработало
    destroyWidget(widget);
    return;
}

void FieldWidget::clearGrid()
{
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            grid[row][col] = nullptr;
        }
    }
    activeWidgets.clear();
}

void FieldWidget::copyGridToTest()
{
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            testGrid[row][col] = grid[row][col];
        }
    }
}

QRect FieldWidget::findWidgetRectInGrid(std::shared_ptr<TemplateFieldWidget> widget,
                                        std::shared_ptr<TemplateFieldWidget> (&grid)[GRID_ROWS][GRID_COLS])
{
    QPoint topLeft(-1, -1);
    for (int row = 0; row < GRID_ROWS; ++row)
    {
        for (int col = 0; col < GRID_COLS; ++col)
        {
            if (grid[row][col] == widget)
            {
                topLeft = QPoint(col, row);
                goto found_top_left;
            }
        }
    }

    return QRect();

found_top_left:

    int left = topLeft.x();
    while (left > 0 && grid[topLeft.y()][left - 1] == widget)
    {
        --left;
    }

    int right = topLeft.x();
    while (right < GRID_COLS - 1 && grid[topLeft.y()][right + 1] == widget)
    {
        ++right;
    }

    int width = right - left + 1;

    int bottom = topLeft.y();
    bool sameWidth = true;
    do
    {
        if (bottom >= GRID_ROWS)
            break;

        int curLeft = left;
        while (curLeft <= right && grid[bottom][curLeft] == widget)
        {
            ++curLeft;
        }

        sameWidth = (curLeft > right);
        if (!sameWidth)
            break;

        ++bottom;
    } while (bottom < GRID_ROWS);

    --bottom;
    int height = bottom - topLeft.y() + 1;

    return QRect(topLeft, QSize(width, height));
}

void FieldWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    cellWidth = this->width() / GRID_COLS;
    cellHeight = this->height() / GRID_ROWS;

    QSize size;
    QPoint pos;
    int col;
    int row;

    for (auto &widget : activeWidgets)
    {
        QRect rect = findWidgetRectInGrid(widget, grid);

        resizeAndMoveWithAnimation(widget, rect.topLeft(), rect.size());
    }
}
